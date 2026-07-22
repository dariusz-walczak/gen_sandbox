#!/usr/bin/env -S uv run

import argparse
import json
import logging
import sys
import typing

import rich.console
import rich.markdown
import rich.theme
import sexpdata

import shared.argparse_types
import shared.error
import shared.json
import shared.markdown
import shared.output
import shared.terms

logging.basicConfig(
    level=logging.DEBUG,
    format="[%(levelname)s] %(message)s",
    stream=sys.stderr)

_LOG = logging.getLogger()

def parse_options(args: list[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser()

    default_format = shared.output.Format.HUMAN
    parser.add_argument(
        "-f", "--format", dest="output_format", choices=[x for x in shared.output.Format],
        type=shared.argparse_types.format_enum, default=default_format,
        help=f"Output format (default: {default_format.name})")

    parser.add_argument(
        "-i", "--input", dest="input_path", metavar="PATH",
        help=(
            "Input json file PATH (the data will be read from the STDIN if the PATH is not"
            " specified)"))

    parser.add_argument(
        "-D", "--no-definition", dest="exclude_definition", action="store_true",
        help="Exclude definition from the output")

    default_log_level = "WARNING"
    parser.add_argument(
        "-l", "--log-level", dest="log_level", choices=logging.getLevelNamesMapping().keys(),
        type=shared.argparse_types.logging_level, default=default_log_level,
        help=f"Logging level (default: {default_log_level})")

    return parser.parse_args(args)

def parse_std_input_json() -> typing.Any:
    try:
        parsed = json.loads(sys.stdin.read())
    except json.JSONDecodeError as e:
        raise shared.error.AppError(
            shared.error.AppError.Codes.InvalidInput,
            "Invalid JSON data provided on the standard input") from e

    return parsed


def parse_file_input_json(file_path: str) -> typing.Any:
    try:
        json_file = open(file_path)
    except OSError as e:
        raise shared.error.AppError(
            shared.error.AppError.Codes.InvalidInput,
            f"Failed to open the json file {file_path}") from e

    try:
        parsed = json.loads(json_file.read())
    except json.JSONDecodeError as e:
        raise shared.error.AppError(
            shared.error.AppError.Codes.InvalidInput,
            f"Failed to deserialize the json file ({file_path})") from e

    return parsed


def render_console_markdown(options: argparse.Namespace, text: str) -> None:
    # Define a custom theme
    custom_theme = rich.theme.Theme({
        "markdown.em": "bold red", # Used for errors indication
        "markdown.strong": "bold white",
        "markdown.code": "dim green",
        "markdown.link_url": "dim",
    })

    # Create a console with the custom theme
    console = rich.console.Console(theme=custom_theme)

    # Render the markdown
    md = rich.markdown.Markdown(text)
    console.print(md)


def print_terms_human_int(
        options: argparse.Namespace,
        terms: typing.Sequence[shared.terms.Term],
        level: int = 1
) -> list[str]:
    lines = []

    for term in terms:
        if term.id is not None:
            anchor_markdown = f"`{{#{term.id}}}`"
        else:
            anchor_markdown = "_{MISSING ID}_"
        item_head_markdown = f"{' '*(level-1)*4}*"
        item_tail_markdown = f"{' '*(level)}"

        if options.exclude_definition:
            if term.title is not None:
                title_markdown = f"__{term.title}__"
            else:
                title_markdown = "_MISSING TERM.TITLE_"
            lines.append(
                f"{item_head_markdown} {title_markdown} {anchor_markdown}")
        else:
            if term.definition:
                definition = term.definition
            else:
                definition = ["_MISSING DEFINITION_"]

            lines.append(f"{item_head_markdown} {definition[0]}")
            for line in definition[1:]:
                lines.append(f"{item_tail_markdown} {line}")
            lines[-1] += f" {anchor_markdown}"

        lines += print_terms_human_int(options, term.children, level+1)

    return lines

def print_terms_human(
        options: argparse.Namespace,
        terms: typing.Sequence[shared.terms.Term]
) -> None:

    lines = print_terms_human_int(options, terms)
    render_console_markdown(options, '\n'.join(lines))

def print_terms_machine(terms : typing.Sequence[shared.terms.Term]) -> None:
    minimized = json.dumps(
        terms, indent=None, separators=(",", ":"), default=shared.json.default_cb)
    print(minimized)

def make_gen_ai_friendly_definition(term: shared.terms.Term) -> str:
    if term.definition:
        definition_lines = [line.strip() for line in term.definition]
    else:
        definition_lines = ["[MISSING DEFINITION]"]
    definition_raw = "\n".join(definition_lines)
    return shared.markdown.genai_friendly_format(definition_raw)

def print_terms_symbolic_int(terms: typing.Sequence[shared.terms.Term]) -> list[list[typing.Any]]:
    output = []

    for term in terms:
        title = term.title if term.title else "<unknown-title>"
        definition = make_gen_ai_friendly_definition(term)

        if term.children:
            optional_subterms = [":subterms", print_terms_symbolic_int(term.children)]
        else:
            optional_subterms = []

        output.append(
            ["term", ":id", term.id, ":name", title, ":definition", definition] +
            optional_subterms)

    return output


def print_terms_symbolic(terms: typing.Sequence[shared.terms.Term]) -> None:
    sexp_data = ["glossary", ":terms"] + [print_terms_symbolic_int(terms)]
    serialized: str = sexpdata.dumps(sexp_data) # type: ignore[no-untyped-call]
    print(serialized)


# Make the terms hierarchy flat (with the parent annotation). Return them as a list of line lists,
# to delegate terms joining to the caller
def print_terms_context_int(
        terms: typing.Sequence[shared.terms.Term],
        parent_title: str | None = None,
        parent_id: str | None = None) -> list[list[str]]:

    output_terms = [] # list of line groups, a single group represents one term
    for term in terms:
        title = term.title if term.title else "<unknown-title>"
        if parent_title is not None and parent_id is not None:
            parent_lines = [f"  Parent: {parent_title} {{{parent_id}}}"]
        elif parent_id is not None:
            parent_lines = [f"  Parent: {{{parent_id}}}"]
        else:
            parent_lines = []
        definition_lines = make_gen_ai_friendly_definition(term).splitlines()
        head_definition_line = next(iter(definition_lines), "<no-definition>")
        term_lines = [
            f"- {title} {{{term.id}}}",
        ] + parent_lines + [
            f"  Definition: {head_definition_line}"
        ] + [
            f"  {line}" for line in definition_lines[1:]
        ]
        output_terms.append(term_lines)
        if term.children:
            output_terms += print_terms_context_int(term.children, term.title, term.id)

    return output_terms


def print_terms_context(terms: typing.Sequence[shared.terms.Term]) -> None:
    output_lines = [
        "BEGIN GLOSSARY CONTEXT",
        "",
        "GLOSSARY INSTRUCTIONS",
        "",
        "Each glossary item is separated by a blank line and contains:",
        "",
        "- Space Separated Title {snake_case_id}            Required. `snake_case_id` is globally"
        " unique and stable.",
        "- Parent: Space Separated Title {snake_case_id}    Optional. Term cross-reference.",
        "- Definition: text                                 Required. May cross-reference terms.",
        "",
        "Cross-references appear as `Space Separated Title {snake_case_id}`. In each such"
        " reference, the `{snake_case_id}` is the authoritative target. Do not infer a different"
        " target from similar title text elsewhere in the glossary."
        " Reference Title variations (plural, abbreviated, or contextually simplified forms) are"
        " intentional and carry meaning — do not normalize or correct them.",
        "",
        "This export may omit referenced terms. Treat unavailable references as opaque labels"
        " unless their definitions are required to complete the requested task; then request the"
        " missing term(s).",
        "",
        "GLOSSARY ITEMS (SUBSET)"
    ]
    terms_sections = print_terms_context_int(terms)

    for section in terms_sections:
        output_lines.append("")
        output_lines.extend(section)

    output_lines.append("")
    output_lines.append("END GLOSSARY CONTEXT")

    print('\n'.join(output_lines))


def json_to_terms(raw_terms: typing.Any) -> tuple[shared.terms.Term, ...]:
    return tuple(shared.terms.Term(**raw_term) for raw_term in raw_terms)


def main(options: argparse.Namespace) -> int:
    if options.input_path is not None:
        raw_terms = parse_file_input_json(options.input_path)
    else:
        raw_terms = parse_std_input_json()
    terms = json_to_terms(raw_terms)

    if options.output_format == shared.output.Format.HUMAN:
        print_terms_human(options, terms)
    elif options.output_format == shared.output.Format.MACHINE:
        print_terms_machine(terms)
    elif options.output_format == shared.output.Format.SYMBOLIC:
        print_terms_symbolic(terms)
    elif options.output_format == shared.output.Format.CONTEXT:
        print_terms_context(terms)

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
