import glob
import logging
import os
import re
import sys
import typing

import pydantic

# NOTE: Keep the TERM_NAME_SINGLELINE_PATTERN_STRING and the TERM_NAME_MULTILINE_PATTERN_STRING
#       constants consistent in terms of all non-whitespace characters.
#       When the pattern changes, update its copy in the docs/conventions/terms-and-definitions.md
#       document (search for 'Python re pattern'):
TERM_NAME_SINGLELINE_PATTERN_STRING = r"[\w \-().:,!?.]+"
TERM_NAME_MULTILINE_PATTERN_STRING = r"[\w\s\-().:,!?.]+"
TERM_ID_PATTERN_STRING = r"[a-zA-Z0-9_]+"

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
# process_input_path
# -> process_input_path_inner
#    -> load_from_path
#        -> load_from_file
#           -> strip_empty_lines
#     -> process_term_directory
#        -> [process_input_path_inner]
# extract_referenced_terms
# -> extract_referenced_terms_inner
#    -> extract_term_references
#    -> [extract_referenced_terms_inner]

def process_input_path(
        options: Options,
        seen_terms: dict[str, Term],
        term_path: str) -> list[Term]:

    if options.max_tree_depth is None:
        max_depth = HARD_DEPTH_LIMIT
    else:
        max_depth = min(options.max_tree_depth, HARD_DEPTH_LIMIT)

    if os.path.isdir(term_path):
        return process_term_directory(max_depth, seen_terms, term_path)
    else:
        return process_input_path_inner(max_depth-1, seen_terms, term_path)


def process_input_path_inner(
        remaining_tree_depth: int,
        seen_terms: dict[str, Term],
        term_path: str) -> list[Term]:

    _LOG.info(f"Processing term path: {term_path}")

    if os.path.isfile(term_path):
        _LOG.debug(f"The term path ({term_path}) is an existing regular file")

        root_path, ext = os.path.splitext(term_path)

        if ext == ".md":
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

    for term_path in sorted(glob.glob(f"{term_dir_path}/*.md")):
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
