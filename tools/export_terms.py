#!/usr/bin/env python3
"""
Export terms definitions from the docs/sources/terms/ directory to a markdown list.
"""

import argparse
import glob
import json
import logging
import os
import re
import sys

import shared.argparse_types
import shared.error

logging.basicConfig(
    level=logging.DEBUG,
    format="[%(levelname)s] %(message)s",
    stream=sys.stderr)

_LOG = logging.getLogger()


def parse_options(args):
    parser = argparse.ArgumentParser()

    parser.add_argument(
        "-t", "--term", nargs="+", action="store", metavar="PATH", dest="term_paths", default=[],
        help="PATH to the term file to be included in the exported term list")

    parser.add_argument(
        "--max-depth", action="store", metavar="DEPTH", dest="max_depth",
        type=shared.argparse_types.positive_int,
        help=(
            "Maximum number of subterm levels to be exported starting from each specified term"
            " (default: <unlimited>)"))

    default_log_level = "WARNING"
    parser.add_argument(
        "-l", "--log-level", dest="log_level", choices=logging.getLevelNamesMapping().keys(),
        type=shared.argparse_types.logging_level, default=default_log_level,
        help=f"Logging level (default: {default_log_level})")

    return parser.parse_args(args)


def build_term(title, anchor):
    _LOG.debug("Building term definition (title=%s, anchor=%s)", title, anchor)

    return {
        "anchor": anchor,
        "title": title,
        "definition": [],
        "children": []
    }

def strip_empty_lines(lines):
    if not lines:
        return []

    front_non_empty_offset = next((i for i, s in enumerate(lines) if s), None)
    if front_non_empty_offset is None:
        return []

    back_non_empty_offset = next((i for i, s in enumerate(reversed(lines)) if s), None)
    assert back_non_empty_offset is not None

    return lines[front_non_empty_offset : len(lines) - back_non_empty_offset]
    

def load_term(term_path):
    basic_header_pattern = re.compile(r"^#+ .+$")
    # NOTE: Update the pattern also in the docs/conventions/terms-and-definitions.md when it
    #       changes.
    anchor_header_pattern = re.compile(
        r"^# (?P<text>[\w \-().:,!?.]+) \{#(?P<anchor>[a-zA-Z0-9_]+)\}\s*$")

    # [ignored lines]
    # # Valid Anchor Header {#valid_anchor_header}
    # [definition lines]
    # # Follow-up Header
    # [ignored lines]

    term = None

    _LOG.info("Opening term file (%s)", term_path)

    with open(term_path, "r", encoding="utf-8") as term_file:
        for line in term_file:
            line = line.rstrip("\n")

            if term is None:
                _LOG.debug("The valid anchor header wasn't yet encountered")

                m = anchor_header_pattern.match(line)

                if m is not None:
                    _LOG.debug("The valid anchor header was just encountered")
                    term = build_term(m.group("text"), m.group("anchor"))
            elif basic_header_pattern.match(line):
                _LOG.debug("The follow-up header was encountered")
                # The definition line list is complete. All remaining lines are ignored.
                break
            else:
                _LOG.debug("A definition line was encountered")
                # A line between the valid anchor header and the first follow-up header
                term["definition"].append(line)

    if term is not None:
        term["definition"] = strip_empty_lines(term["definition"])

    return term


# Returns list of terms in the directory (can be assigned to the children of the parent term or
#  be used directly)
def process_term_directory(options, term_dir_path):
    _LOG.info("Processing term directory: %s", term_dir_path)

    result_terms = []

    for term_path in glob.glob(f"{term_dir_path}/*.md"):
        term = process_term_path(options, term_path)

        if term is not None:
            result_terms += term

    return result_terms


def process_term_path(options, term_path):
    _LOG.info("Processing term path: %s", term_path)

    if os.path.isfile(term_path):
        _LOG.debug("The term path (%s) is an existing regular file", term_path)

        root_path, ext = os.path.splitext(term_path)

        if ext == ".md":
            _LOG.debug("The term path (%s) is a markdown file", term_path)

            term = load_term(term_path)

            if term is None:
                _LOG.error("The term file (%s) is not valid", term_path)
                return []

            if os.path.isdir(root_path):
                _LOG.debug("The term directory (%s) exists", root_path)

                term["children"] = process_term_directory(options, root_path)

            return [term]

        _LOG.debug("The term path (%s) is not a markdown file", term_path)
    elif os.path.isdir(term_path):
        _LOG.debug("The term path (%s) is a directory", term_path)

        return process_term_directory(options, term_path)
    else:
        _LOG.debug("The term path (%s) is not an existing file nor a directory", term_path)

    return []

def main(options):
    terms = []

    for term_path in options.term_paths:
        terms += process_term_path(options, term_path)

    print(json.dumps(terms, indent=2))

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
