#!/usr/bin/env python3

import argparse
import enum
import json
import logging
import sys

import colorama

import shared.argparse_types
import shared.error
import shared.output

logging.basicConfig(
    level=logging.DEBUG,
    format="[%(levelname)s] %(message)s",
    stream=sys.stderr)

_LOG = logging.getLogger()

def parse_options(args):
    parser = argparse.ArgumentParser()

    default_format = shared.output.Format.HUMAN
    parser.add_argument(
        "-f", "--format", dest="output_format", choices=[x for x in shared.output.Format],
        type=shared.argparse_types.format_enum, default=default_format,
        help=f"Output format (default: {default_format.name})")

    parser.add_argument(
        "-D", "--no-definition", dest="exclude_definition", action="store_true",
        help="Exclude definition from the output")

    default_log_level = "WARNING"
    parser.add_argument(
        "-l", "--log-level", dest="log_level", choices=logging.getLevelNamesMapping().keys(),
        type=shared.argparse_types.logging_level, default=default_log_level,
        help=f"Logging level (default: {default_log_level})")

    return parser.parse_args(args)

def parse_std_input_json():
    try:
        parsed = json.loads(sys.stdin.read())
    except json.JSONDecodeError as e:
        raise shared.error.AppError(
            shared.error.AppError.Codes.InvalidInput,
            f"Invalid JSON data provided on the standard input") from e

    return parsed

###################################################################################################

from rich.markdown import Markdown
from rich.console import Console
from rich.theme import Theme

def render_console_markdown(options, text):
    # Define a custom theme
    custom_theme = Theme({
        "markdown.bold": "bold red",          # Bold text in red
        "markdown.link_url": "dim",           # URL part (hidden or dimmed)
    })

    # Create a console with the custom theme
    console = Console(theme=custom_theme)

    # Render the markdown
    md = Markdown(text)
    console.print(md)

###################################################################################################

def print_terms_human_int(options, terms, level=1):
    lines = []

    for term in terms:
        title = term.get("title", "<unknown-title>")
        anchor = term.get("anchor", "<unknown-anchor>")
        lines.append(
            f"{' '*(level-1)*4}* "
            f"{colorama.Fore.BLUE}{colorama.Style.BRIGHT}{title}{colorama.Style.RESET_ALL} "
            f"{{{colorama.Fore.GREEN}{colorama.Style.DIM}#{anchor}{colorama.Style.RESET_ALL}}}")

        if not options.exclude_definition:
            # Add two spaces after the title/anchor line to force line break before the definition:
            lines[-1] += "  "
            for line in term.get("definition", "<no-definition>"):
                lines.append(f"{' '*(level)} {line}")

        lines += print_terms_human_int(options, term["children"], level+1)

    return lines

def print_terms_human(options, terms):
    lines = print_terms_human_int(options, terms)
    render_console_markdown(options, '\n'.join(lines))

def main(options):
    data = parse_std_input_json()

    if options.output_format == shared.output.Format.HUMAN:
        print_terms_human(options, data)


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
