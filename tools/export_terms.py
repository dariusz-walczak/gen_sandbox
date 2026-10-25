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
    HIERARCHY_MODE: str = "--hierarchy"
    DEPENDENCY_MODE: str = "--dependency"
    TREE_ROOT: str = "--tree-root"
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
                "Export Term Information from a Filesystem Term Repository as a JSON Document.",
                "",
                "The script operates in one of two modes:",
                "",
                "__Hierarchy Mode__ (default)",
            )
        )
        console.print(
            _make_panel(
                f"The Hierarchy Mode is triggered by the `{OptionNames.HIERARCHY_MODE}` option.",
                "",
                "In this mode, the program exports the Term Hierarchy Information as Term",
                " Subtrees. Parent-Child Relations are represented by nesting child Term Data",
                f" within parent Term Data. Subtrees start at Start Terms (specified via `{OptionNames.TERM}`",
                " options) or the Tree Root if none are provided.",
                "",
                f"The Tree Depth Limit is controlled by `{OptionNames.MAX_TREE_DEPTH}`",
                " (Start Terms count as level 1).",
            )
        )
        console.print(
            _make_markdown(
                "__Dependency Mode__",
            )
        )
        console.print(
            _make_panel(
                f"The Dependency Mode is triggered by the `{OptionNames.DEPENDENCY_MODE}` option.",
                "",
                "In this mode, the program exports the Term Dependency Information as a flat list",
                " of Terms. Each Term appears at most once, in the order they are visited during",
                " traversal of Reference Paths. Requires at least one Start Term",
                f"The Reference Depth Limit is controlled by `{OptionNames.MAX_REF_DEPTH}`",
                " (Start Terms count as level 1).",
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
        "-r", OptionNames.TREE_ROOT, action="store", metavar="PATH", dest="input_path",
        default=default_input_path,
        help=(f"PATH to the Term Tree root file or directory (default: {default_input_path})"))

    parser.add_argument(
        "-t", OptionNames.TERM, nargs="+", action="extend", metavar="ID", dest="term_ids",
        default=[], help="IDs of Start Terms to begin traversal from")

    mode_group = parser.add_mutually_exclusive_group(required=False)
    mode_group.add_argument(
        "-H", OptionNames.HIERARCHY_MODE, action="store_true", dest="hierarchy_mode",
        help="Trigger the Hierarchy Mode (default)")
    mode_group.add_argument(
        "-D", OptionNames.DEPENDENCY_MODE, action="store_true", dest="dependency_mode",
        help="Trigger the Dependency Mode")

    default_max_tree_depth = shared.term.HARD_DEPTH_LIMIT
    parser.add_argument(
        OptionNames.MAX_TREE_DEPTH, action="store", metavar="LEVEL", dest="max_tree_depth",
        default=default_max_tree_depth,
        type=shared.argparse_types.make_ranged_int(1, shared.term.HARD_DEPTH_LIMIT),
        help=(
            "Tree Depth Limit: maximum number of levels in Term Subtree traversal"
            f" (min: 1, max: {shared.term.HARD_DEPTH_LIMIT}, default:"
            f" {default_max_tree_depth}). This option is ineffective in the Dependency Mode"))

    default_max_ref_depth = min(3, shared.term.HARD_DEPTH_LIMIT)
    parser.add_argument(
        OptionNames.MAX_REF_DEPTH, action="store", metavar="LEVEL", dest="max_ref_depth",
        default=default_max_ref_depth,
        type=shared.argparse_types.make_ranged_int(1, shared.term.HARD_DEPTH_LIMIT),
        help=(
            "Reference Depth Limit: maximum number of levels in Reference Path traversal"
            f" (min: 1, max: {shared.term.HARD_DEPTH_LIMIT}, default:"
            f" {default_max_ref_depth}). This option is ineffective in the Hierarchy Mode"))

    default_log_level = "WARNING"
    parser.add_argument(
        "-l", "--log-level", dest="log_level", choices=logging.getLevelNamesMapping().keys(),
        type=shared.argparse_types.logging_level, default=default_log_level,
        help=f"Logging level (default: {default_log_level})")

    options = parser.parse_args(args)

    # Apply the log level optionally provided through command line argument:
    level = logging.getLevelNamesMapping().get(options.log_level, logging.NOTSET)
    logging.getLogger().setLevel(level)

    # Select the default mode when no mode is selected:
    if not options.hierarchy_mode and not options.dependency_mode:
        _LOG.debug("No mode flag provided, assuming Hierarchy Mode")
        options.hierarchy_mode = True

    # Prevent the value of the max tree depth to be overridden when the dependency mode is
    #  selected:
    if options.dependency_mode and options.max_tree_depth != default_max_tree_depth:
        _LOG.warning(
            f"The {OptionNames.MAX_TREE_DEPTH} option change is ineffective in the Dependency Mode"
            f" (the limit stays at its default {default_max_tree_depth})")
        options.max_tree_depth = default_max_tree_depth

    # Print a warning just for UX and consistency with the max tree depth option:
    if options.hierarchy_mode and options.max_ref_depth != default_max_ref_depth:
        _LOG.warning(
            f"The {OptionNames.MAX_REF_DEPTH} option change is ineffective in the Hierarchy Mode"
            f" (the limit stays at its default {default_max_ref_depth})")
        options.max_ref_depth = default_max_ref_depth

    return options


def main(options: argparse.Namespace) -> int:
    _LOG.debug(
        f"hierarchy_mode={options.hierarchy_mode}, dependency_mode={options.dependency_mode}")

    if options.dependency_mode and not options.term_ids:
        raise shared.error.AppError(
            shared.error.AppError.Codes.UserError,
            f"Dependency Mode requires at least one Start Term (use the {OptionNames.TREE_ROOT}"
            " option)")

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

    if options.dependency_mode:
        terms_collection = shared.term.extract_referenced_terms(
            term_options, options.term_ids, terms_lookup)
        print(json.dumps(terms_collection, indent=2, default=shared.json.default_cb))
    else:
        print(json.dumps(term_trees, indent=2, default=shared.json.default_cb))

    return 0


if __name__ == '__main__':
    options = parse_options(sys.argv[1:])

    try:
        sys.exit(main(options))
    except shared.error.AppError as e:
        print(f"ERROR: {e}")
        sys.exit(1)
