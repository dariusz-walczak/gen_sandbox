#!/usr/bin/env -S uv run

import argparse
import itertools
import json
import logging
import os
import sys

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

_LOG = logging.getLogger(__name__)


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
        "-t", OptionNames.TERM, nargs="+", action="extend", metavar="ID", dest="term_ids",
        default=[], help="IDs of root terms to be included in the exported term list")

    mode_group = parser.add_mutually_exclusive_group(required=False)
    mode_group.add_argument(
        "-H", "--hierarchy", action="store_true", dest="hierarchy_mode",
        help="Trigger the Hierarchy Mode (default)")
    mode_group.add_argument(
        "-R", "--reference", action="store_true", dest="reference_mode",
        help="Trigger the Reference Mode")

    default_max_tree_depth = shared.term.HARD_DEPTH_LIMIT
    parser.add_argument(
        OptionNames.MAX_TREE_DEPTH, action="store", metavar="LEVEL", dest="max_tree_depth",
        default=default_max_tree_depth,
        type=shared.argparse_types.make_ranged_int(1, shared.term.HARD_DEPTH_LIMIT),
        help=(
            "Maximum number of term tree LEVELs to be processed starting from the specified input"
            f" path (min: 1, max: {shared.term.HARD_DEPTH_LIMIT}, default:"
            f" {default_max_tree_depth})"))

    default_max_ref_depth = min(3, shared.term.HARD_DEPTH_LIMIT)
    parser.add_argument(
        OptionNames.MAX_REF_DEPTH, action="store", metavar="LEVEL", dest="max_ref_depth",
        default=default_max_ref_depth,
        type=shared.argparse_types.make_ranged_int(1, shared.term.HARD_DEPTH_LIMIT),
        help=(
            "Maximum number of term reference LEVELs to be processed starting from each specified"
            f" root term (min: 1, max: {shared.term.HARD_DEPTH_LIMIT}, default:"
            f" {default_max_ref_depth})"))

    default_log_level = "WARNING"
    parser.add_argument(
        "-l", "--log-level", dest="log_level", choices=logging.getLevelNamesMapping().keys(),
        type=shared.argparse_types.logging_level, default=default_log_level,
        help=f"Logging level (default: {default_log_level})")

    options = parser.parse_args(args)

    if not options.hierarchy_mode and not options.reference_mode:
        options.hierarchy_mode = True

    return options


def main(options: argparse.Namespace) -> int:
    _LOG.debug(f"hierarchy_mode={options.hierarchy_mode}, reference_mode={options.reference_mode}")

    # The terms lookup table is used for de-duplication when passed to the process_input_path.
    # It is later reused for term reference resolution when passed to the extract_referenced_terms.
    terms_lookup: dict[str, shared.term.Term] = {}

    term_options = shared.term.Options(
        max_tree_depth=options.max_tree_depth,
        max_ref_depth=options.max_ref_depth)

    term_paths: list[str] = []

    if options.hierarchy_mode and options.term_ids:
        relative_term_paths = shared.term.find_term_file_paths(
            options.input_path, options.term_ids)
        term_paths.extend(
            os.path.join(options.input_path, term_path) for term_path in relative_term_paths)
    else:
        term_paths.append(options.input_path)

    term_trees: list[shared.term.Term] = list(
        itertools.chain.from_iterable(
            shared.term.process_input_path(term_options, terms_lookup, term_path)
            for term_path in term_paths))

    if options.reference_mode:
        terms_collection = shared.term.extract_referenced_terms(
            term_options, options.term_ids, terms_lookup)
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
