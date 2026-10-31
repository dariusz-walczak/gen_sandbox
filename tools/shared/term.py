import glob
import logging
import os
import re
import sys
import typing

import pydantic

import shared.error

# NOTE: Keep the TERM_NAME_SINGLELINE_PATTERN_STRING and the TERM_NAME_MULTILINE_PATTERN_STRING
#       constants consistent in terms of all non-whitespace characters.
#       When the pattern changes, update its copy in the docs/conventions/terms-and-definitions.md
#       document (search for 'Python re pattern'):
TERM_NAME_SINGLELINE_PATTERN_STRING = r"[\w \-().:,!?.]+"
TERM_NAME_MULTILINE_PATTERN_STRING = r"[\w\s\-().:,!?.]+"
TERM_ID_PATTERN_STRING = r"[a-zA-Z0-9_]+"
TERM_FILE_EXTENSION = ".md"

_LOG = logging.getLogger(__name__)


class Term(pydantic.BaseModel):
    id: str
    title: str | None = None
    path: str
    definition: list[str] = []
    children: list["Term"] = []

HARD_DEPTH_LIMIT = sys.getrecursionlimit() // 4


class Options(pydantic.BaseModel):
    max_tree_depth: int | None = None
    max_ref_depth: int | None = None


# Local function call tree:
#
# find_term_file_path
# -> load_from_path
#
# process_input_path
# -> process_input_path_inner
#    -> load_from_path
#        -> load_from_file
#           -> strip_empty_lines
#     -> process_term_directory
#        -> [process_input_path_inner]
#
# extract_referenced_terms
# -> extract_referenced_terms_inner
#    -> extract_term_references
#    -> [extract_referenced_terms_inner]

# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - #

def find_term_file_paths(term_tree_root: str, term_ids: typing.Sequence[str]) -> list[str]:
    unfiltered_paths: list[str | None] = [
        find_term_file_path(term_tree_root, term_id) for term_id in term_ids]
    return [p for p in unfiltered_paths if p is not None]


def find_term_file_path(term_tree_root: str, term_id: str) -> str | None:
    """Search the term tree for a specific term

    The term tree is walked depth-first starting from the root path. Sub-directories are descended
    into only when they correspond to a term file in the current directory (e.g. `parent/child/`
    is entered only if `parent/child.md` exists).

    :param term_tree_root: the root path of the term tree to be searched
    :param term_id: the identifier of the term to be searched for
    :returns: the term tree root relative path to the term file or None if it was not found
    """

    _LOG.info(f"Searching for the term ({term_id}) starting from the root path ({term_tree_root})")

    # Input Contract Enforcement:

    if not term_id:
        raise shared.error.AppError(
            shared.error.AppError.Codes.InputContractError,
            "The term identifier is not specified")

    if not re.fullmatch(TERM_ID_PATTERN_STRING, term_id):
        raise shared.error.AppError(
            shared.error.AppError.Codes.InputContractError,
            f"The term identifier ({term_id}) is invalid")

    if not term_tree_root:
        raise shared.error.AppError(
            shared.error.AppError.Codes.InputContractError,
            "The term tree root path is not specified")

    if not os.path.exists(term_tree_root):
        raise shared.error.AppError(
            shared.error.AppError.Codes.InputContractError,
            f"The term tree root path ({term_tree_root}) doesn't exist")

    # The root path is a file:

    if os.path.isfile(term_tree_root):
        if term_tree_root.endswith(TERM_FILE_EXTENSION):
            term = load_from_path(term_tree_root)

            if term is None:
                raise shared.error.AppError(
                    shared.error.AppError.Codes.InvalidInput,
                    f"The term tree root file ({term_tree_root}) is not a valid term file")
            elif term.id == term_id:
                return term_tree_root
            else:
                return find_term_file_path(
                    term_tree_root.removesuffix(TERM_FILE_EXTENSION), term_id)
        else:
            raise shared.error.AppError(
                shared.error.AppError.Codes.InvalidInput,
                f"Invalid term tree root file ({term_tree_root}) extension (expected:"
                f" {TERM_FILE_EXTENSION})")

    # The root path is a directory:

    if not os.path.isdir(term_tree_root):
        raise shared.error.AppError(
            shared.error.AppError.Codes.InputContractError,
            f"The term tree root path ({term_tree_root}) is not a directory")

    for curr_dir_path, curr_dir_names, curr_file_names in os.walk(term_tree_root):
        markdown_file_names = [fn for fn in curr_file_names if fn.endswith(TERM_FILE_EXTENSION)]
        acceptable_dir_names = set()

        for markdown_file_name in markdown_file_names:
            term_path = os.path.join(curr_dir_path, markdown_file_name)
            term = load_from_path(term_path)

            if term is not None:
                if term.id == term_id:
                    return term_path  # Term found
                else:
                    acceptable_dir_names.add(markdown_file_name.removesuffix(TERM_FILE_EXTENSION))

        curr_dir_names[:] = [dn for dn in curr_dir_names if dn in acceptable_dir_names]

    return None  # Term not found


# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - #

def process_input_path(
        options: Options, seen_terms: dict[str, Term], term_path: str) -> list[Term]:

    if options.max_tree_depth is None:
        max_depth = HARD_DEPTH_LIMIT
    else:
        max_depth = min(options.max_tree_depth, HARD_DEPTH_LIMIT)

    if os.path.isdir(term_path):
        return process_term_directory(max_depth, seen_terms, term_path)
    else:
        return process_input_path_inner(max_depth-1, seen_terms, term_path)


def process_input_path_inner(
        remaining_tree_depth: int, seen_terms: dict[str, Term], term_path: str) -> list[Term]:

    _LOG.info(f"Processing term path: {term_path}")

    if os.path.isfile(term_path):
        _LOG.debug(f"The term path ({term_path}) is an existing regular file")

        root_path, ext = os.path.splitext(term_path)

        if ext == TERM_FILE_EXTENSION:
            _LOG.debug(f"The term path ({term_path}) is a markdown file")

            term = load_from_path(term_path)

            if term is None:
                _LOG.error(f"The term file ({term_path}) is not valid")
                return []

            if term.id in seen_terms:
                _LOG.warning(
                    f"Duplicate definition for term '{term.id}' found at '{term.path}'. First"
                    f" occurrence at '{seen_terms[term.id].path}'. The duplicate and its children"
                    " are ignored.")
                return []
            else:
                seen_terms[term.id] = term

            if os.path.isdir(root_path):
                _LOG.debug("The term directory (%s) exists", root_path)

                term.children = process_term_directory(
                    remaining_tree_depth, seen_terms, root_path)

            return [term]

        _LOG.debug(f"The term path ({term_path}) is not a markdown file")
    elif os.path.isdir(term_path):
        _LOG.debug(f"The term path ({term_path}) is a directory")

        return process_term_directory(remaining_tree_depth, seen_terms, term_path)
    else:
        _LOG.debug(f"The term path ({term_path}) is not an existing file nor a directory")

    return []


# Returns list of terms in the directory (can be assigned to the children of the parent term or
#  be used directly)
def process_term_directory(
        remaining_tree_depth: int,
        seen_terms: dict[str, Term],
        term_dir_path: str) -> list[Term]:

    if remaining_tree_depth <= 0:
        _LOG.debug(f"Max tree depth reached - skipping term directory: {term_dir_path}")
        return []

    _LOG.info(f"Processing term directory: {term_dir_path}")

    result_terms = []

    for term_path in sorted(glob.glob(f"{term_dir_path}/*{TERM_FILE_EXTENSION}")):
        result_terms += process_input_path_inner(remaining_tree_depth-1, seen_terms, term_path)

    return result_terms


def load_from_path(term_path: str) -> Term | None:
    _LOG.info("Opening term file (%s)", term_path)

    with open(term_path, encoding="utf-8") as term_file:
        return load_from_file(term_file, term_path)


def load_from_file(term_file: typing.IO[str], term_path: str) -> Term | None:
    # [ignored lines]
    # # Valid Anchor Header {#valid_anchor_header}
    # [definition lines]
    # # Follow-up Header
    # [ignored lines]

    basic_header_pattern = re.compile(r"^#+ .+$")

    name_pattern_str = TERM_NAME_SINGLELINE_PATTERN_STRING
    id_pattern_str = TERM_ID_PATTERN_STRING

    anchor_header_pattern = re.compile(
        rf"^# (?P<name>{name_pattern_str}) \{{#(?P<id>{id_pattern_str})\}}\s*$")

    _LOG.debug(f"Reading the term file ({term_path})")

    term = None

    for line in term_file:
        line = line.rstrip("\n")

        if term is None:
            _LOG.debug("The valid anchor header wasn't yet encountered")

            m = anchor_header_pattern.match(line)

            if m is not None:
                _LOG.debug("The valid anchor header was just encountered")

                term = Term(id=m.group("id"), title=m.group("name"), path=term_path)
        elif basic_header_pattern.match(line):
            _LOG.debug("The follow-up header was encountered")
            # The definition line list is complete. All remaining lines are ignored.
            break
        else:
            _LOG.debug("A definition line was encountered")
            # A line between the valid anchor header and the first follow-up header
            term.definition.append(line)

    if term is not None:
        term.definition = strip_empty_lines(term.definition)

    return term


def strip_empty_lines(lines: list[str]) -> list[str]:
    """Remove empty lines from the front and the end of the provided line list"""
    if not lines:
        return []

    front_non_empty_offset = next((i for i, s in enumerate(lines) if s), None)
    if front_non_empty_offset is None:
        return []  # The term contains empty lines only

    back_non_empty_offset = next((i for i, s in enumerate(reversed(lines)) if s), None)
    assert back_non_empty_offset is not None # Excluded by the front_non_empty_offset is None check

    return lines[front_non_empty_offset : len(lines) - back_non_empty_offset]


def extract_referenced_terms(
        options: Options,
        term_ids: typing.Sequence[str],
        term_lookup: dict[str, Term],
        seen_terms: set[str] | None = None) -> list[Term]:

    if options.max_ref_depth is None:
        max_depth = HARD_DEPTH_LIMIT
    else:
        max_depth = min(options.max_ref_depth, HARD_DEPTH_LIMIT)

    return extract_referenced_terms_inner(max_depth, term_ids, term_lookup, seen_terms)


def extract_referenced_terms_inner(
        remaining_ref_depth: int,
        term_ids: typing.Sequence[str],
        term_lookup: dict[str, Term],
        seen_terms: set[str] | None = None) -> list[Term]:

    if remaining_ref_depth <= 0:
        _LOG.debug("Max reference depth reached - skipping references resolution")
        return []

    if seen_terms is None:
        seen_terms = set()

    extracted_terms = []

    for term_id in term_ids:
        _LOG.info(f"term_id={term_id}, remaining_ref_depth={remaining_ref_depth}")

        if term_id not in term_lookup:
            _LOG.warning(f"Requested term ({term_id}) wasn't found in the specified term trees")

            continue

        term = term_lookup[term_id]

        if term_id in seen_terms:
            _LOG.debug(f"Term ({term_id}) already extracted")
        else:
            # Make a shallow copy of `term` with the `children` list cleared to not pollute the
            #  result list with unreferenced subterms of a referenced term:
            extracted_terms.append(term.model_copy(update={"children": []}))
            seen_terms.add(term_id)

        # References are followed even for already-seen terms because the tree traversal is
        #  depth-first. If a term was first reached via a path that consumed more of the depth
        #  budget, fewer levels remained for its transitive references. Re-processing via a
        #  shallower path ensures all reachable referenced terms are included.
        # If this solution's performance becomes an issue, a more sophisticated algorithm may need
        #  to be implemented.
        referenced_terms = extract_term_references(term)
        _LOG.info(
            f"Terms referenced by the {term.id} definition: "
            f"{', '.join(referenced_terms)}")
        extracted_terms += extract_referenced_terms_inner(
            remaining_ref_depth-1, referenced_terms, term_lookup, seen_terms)

    return extracted_terms


def extract_term_references(term: Term) -> list[str]:
    name_pattern_str = TERM_NAME_MULTILINE_PATTERN_STRING
    id_pattern_str = TERM_ID_PATTERN_STRING

    definition_text = "\n".join(term.definition)
    term_id_pattern = re.compile(
        rf"\[(?:{name_pattern_str})\]\(#(?P<id>{id_pattern_str})\)")
    return [m.group("id") for m in term_id_pattern.finditer(definition_text)]
