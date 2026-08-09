#!/usr/bin/env -S uv run

import argparse
import dataclasses
import glob
import json
import logging
import os
import re
import sys
import typing

import rich.box
import rich.console
import rich.markdown
import rich.panel
import rich.theme

import shared.argparse_types
import shared.error
import shared.json
import shared.term

logging.basicConfig(
    level=logging.INFO,
    format="[%(levelname)s] %(message)s",
    stream=sys.stderr)

_LOG = logging.getLogger()


def get_default_input_path() -> str:
    tools_dir = os.path.dirname(os.path.abspath(__file__))
    terms_dir = os.path.join(tools_dir, "../docs/sources/terms")
    return os.path.relpath(terms_dir)

class OptionNames:
    HIERARCHY: str = "--root"
    TERM: str = "--term"
    MAX_REF_DEPTH: str = "--max-reference-depth"
    MAX_TREE_DEPTH: str = "--max-tree-depth"

def render_argparse_description() -> str:
    custom_theme = rich.theme.Theme({
        "markdown.em": "red",
        "markdown.strong": "bold white",
        "markdown.code": "green",
    })

    console = rich.console.Console(theme=custom_theme)

    def _make_markdown(*lines: str) -> rich.markdown.Markdown:
        return rich.markdown.Markdown("\n".join(lines))

    def _make_panel(*lines: str) -> rich.panel.Panel:
        return rich.panel.Panel(
            rich.markdown.Markdown("\n".join(lines)),
            box=rich.box.MINIMAL)

    with console.capture() as capture:
        console.print(
            _make_markdown(
                "Export project terms from the term repository to JSON format in one of two",
                " modes.",
                "",
                "__Hierarchy Mode__ (default)",
            )
        )
        console.print(
            _make_panel(
                f"The hierarchy mode is triggered by the absence of the `{OptionNames.TERM}`",
                " option.",
                "",
                "In this mode, the program exports the term repository as a list of term",
                " subtrees.",
                "The number of exported subtrees depends on the type of the filesystem node",
                f" pointed by the `{OptionNames.HIERARCHY}` option:",
                "* A single subtree is exported when the option points to a term file.",
                f"* When the `{OptionNames.HIERARCHY}` option points to a directory (default),",
                " one subtree is exported per term file located in the directory.",
                "",
                f"The depth of each subtree is controlled by `{OptionNames.MAX_TREE_DEPTH}`"
                " (roots count as level 1).",
            )
        )
        console.print(
            _make_markdown(
                "__Reference Mode__",
            )
        )
        console.print(
            _make_panel(
                "The reference mode is triggered by the presence of at least one",
                f" `{OptionNames.TERM}` option.",
                "",
                "In this mode, the program exports a flat list of the root terms specified by the",
                f" `{OptionNames.TERM}` option(s), together with all terms recursively referenced",
                " by those terms.",
                "",
                f"The maximum reference depth is controlled by the `{OptionNames.MAX_REF_DEPTH}`",
                " option (roots count as level 1).",
            )
        )
        console.print(
            _make_markdown(
                "The output of this command is typically piped to the `print_tree.py` tool, which",
                " prints the exported data in human-, machine-, or AI-friendly formats."
            )
        )

    return capture.get()


def parse_options(args: list[str]) -> argparse.Namespace:
    desc = render_argparse_description()

    parser = argparse.ArgumentParser(
        description = desc,
        formatter_class=argparse.RawDescriptionHelpFormatter
    )

    default_input_path = get_default_input_path()
    parser.add_argument(
        "-i", OptionNames.HIERARCHY, action="store", metavar="PATH", dest="input_path",
        default=default_input_path,
        help=(f"PATH to the term tree root file or directory (default: {default_input_path})"))

    parser.add_argument(
        "-t", OptionNames.TERM, nargs="+", action="extend", metavar="ID", dest="term_ids", default=[],
        help="IDs of root terms to be included in the exported term list")

    parser.add_argument(
        OptionNames.MAX_TREE_DEPTH, action="store", metavar="LEVEL", dest="max_tree_depth",
        type=shared.argparse_types.positive_int,
        help=(
            "Maximum number of term tree LEVELs to be processed starting from the specified input"
            " path (default: <unlimited>)"))

    parser.add_argument(
        OptionNames.MAX_REF_DEPTH, action="store", metavar="LEVEL", dest="max_cref_depth",
        type=shared.argparse_types.positive_int,
        help=(
            "Maximum number of term reference LEVELs to be processed starting from each specified"
            " root term (default: <unlimited>)"))

    default_log_level = "WARNING"
    parser.add_argument(
        "-l", "--log-level", dest="log_level", choices=logging.getLevelNamesMapping().keys(),
        type=shared.argparse_types.logging_level, default=default_log_level,
        help=f"Logging level (default: {default_log_level})")

    return parser.parse_args(args)


def load_term(term_path: str) -> shared.term.Term | None:
    _LOG.info("Opening term file (%s)", term_path)

    with open(term_path, encoding="utf-8") as term_file:
        return shared.term.load(term_file, term_path)


# Returns list of terms in the directory (can be assigned to the children of the parent term or
#  be used directly)
def process_term_directory(
        options: argparse.Namespace,
        seen_terms: dict[str, shared.term.Term],
        term_dir_path: str
) -> list[shared.term.Term]:

    _LOG.info("Processing term directory: %s", term_dir_path)

    result_terms = []

    for term_path in glob.glob(f"{term_dir_path}/*.md"):
        term = process_input_path(options, seen_terms, term_path)

        if term is not None:
            result_terms += term

    return result_terms


def process_input_path(
        options: argparse.Namespace,
        seen_terms: dict[str, shared.term.Term],
        term_path: str
) -> list[shared.term.Term]:

    _LOG.info(f"Processing term path: {term_path}")

    if os.path.isfile(term_path):
        _LOG.debug(f"The term path ({term_path}) is an existing regular file")

        root_path, ext = os.path.splitext(term_path)

        if ext == ".md":
            _LOG.debug(f"The term path ({term_path}) is a markdown file")

            term = load_term(term_path)

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

                term.children = process_term_directory(options, seen_terms, root_path)

            return [term]

        _LOG.debug(f"The term path ({term_path}) is not a markdown file")
    elif os.path.isdir(term_path):
        _LOG.debug(f"The term path ({term_path}) is a directory")

        return process_term_directory(options, seen_terms, term_path)
    else:
        _LOG.debug(f"The term path ({term_path}) is not an existing file nor a directory")

    return []


def extract_term_references(term: shared.term.Term) -> list[str]:
    name_pattern_str = shared.term.TERM_NAME_MULTILINE_PATTERN_STRING
    id_pattern_str = shared.term.TERM_ID_PATTERN_STRING

    definition_text = "\n".join(term.definition)
    term_id_pattern = re.compile(
        rf"\[(?:{name_pattern_str})\]\(#(?P<id>{id_pattern_str})\)")
    return [m.group("id") for m in term_id_pattern.finditer(definition_text)]


def extract_referenced_terms(
        term_ids: typing.Sequence[str],
        terms_lookup: dict[str, shared.term.Term],
        seen_terms: set[str] | None = None
) -> list[shared.term.Term]:

    extracted_terms = []
    if seen_terms is None:
        seen_terms = set()

    for term_id in term_ids:
        if term_id in seen_terms:
            _LOG.debug(f"Term ({term_id}) already extracted")
        elif term_id not in terms_lookup:
            _LOG.warning(f"Requested term ({term_id}) wasn't found in the specified term trees")
        else:
            term = terms_lookup[term_id]
            # Make a shallow copy of `term` with the `children` list cleared to not pollute the
            #  result list with unreferenced subterms of a referenced term:
            extracted_terms.append(term.model_copy(update={"children": []}))
            seen_terms.add(term_id)
            referenced_terms = extract_term_references(term)
            _LOG.info(
                f"Terms referenced by the {term.id} definition: "
                f"{', '.join(referenced_terms)}")
            extracted_terms += extract_referenced_terms(
                referenced_terms, terms_lookup, seen_terms)

    return extracted_terms


def main(options: argparse.Namespace) -> int:
    # The terms lookup table is used for de-duplication when passed to the process_input_path.
    # It is later reused for term reference resolution when passed to the extract_referenced_terms.
    terms_lookup: dict[str, shared.term.Term] = {}

    term_trees: list[shared.term.Term] = process_input_path(
        options, terms_lookup, options.input_path)

    if options.term_ids:
        terms_collection = extract_referenced_terms(options.term_ids, terms_lookup)
        print(json.dumps(terms_collection, indent=2, default=shared.json.default_cb))
    else:
        print(json.dumps(term_trees, indent=2, default=shared.json.default_cb))

    return 0


if __name__ == '__main__':
    options = parse_options(sys.argv[1:])

    # Apply the log level optionally provided through command line argument:
    level = logging.getLevelNamesMapping().get(options.log_level, logging.NOTSET)
    logging.getLogger().setLevel(level)

    try:
        sys.exit(main(options))
    except shared.error.AppError as e:
        print(f"ERROR: {e}")
        sys.exit(1)
