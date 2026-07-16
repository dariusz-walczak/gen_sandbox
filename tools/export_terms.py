#!/usr/bin/env -S uv run

import argparse
import glob
import json
import logging
import os
import re
import sys

import shared.argparse_types
import shared.error
import shared.terms

logging.basicConfig(
    level=logging.DEBUG,
    format="[%(levelname)s] %(message)s",
    stream=sys.stderr)

_LOG = logging.getLogger()


def parse_options(args):
    parser = argparse.ArgumentParser()

    parser.add_argument(
        "-i", "--input", nargs="+", action="store", metavar="PATH", dest="input_paths", default=[],
        help="PATH to the term file to be included in the exported term list")

    parser.add_argument(
        "-t", "--term", nargs="+", action="store", metavar="ID", dest="term_ids", default=[],
        help="ID of the root term to be included in the exported term list")

    parser.add_argument(
        "--max-tree-depth", action="store", metavar="NUMBER", dest="max_tree_depth",
        type=shared.argparse_types.positive_int,
        help=(
            "Maximum NUMBER of term hierarchy levels to be extracted starting from each specified"
            " input path (default: <unlimited>)"))

    parser.add_argument(
        "--max-cref-depth", action="store", metavar="DEPTH", dest="max_cref_depth",
        type=shared.argparse_types.positive_int,
        help=(
            "Maximum NUMBER of cross-references to be followed starting from each specified term"
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


def strip_children(term):
    """shallow copy: the definition list is referenced rather then copied. the main purpose is to optionally exclude
    children"""
    return {**term, "children": []}


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

    name_pattern_str = shared.terms.TERM_NAME_SINGLELINE_PATTERN_STRING
    id_pattern_str = shared.terms.TERM_ID_PATTERN_STRING

    anchor_header_pattern = re.compile(
        rf"^# (?P<name>{name_pattern_str}) \{{#(?P<id>{id_pattern_str})\}}\s*$")

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
                    term = build_term(m.group("name"), m.group("id"))
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
        term = process_input_path(options, term_path)

        if term is not None:
            result_terms += term

    return result_terms


def process_input_path(options, term_path):
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


def build_terms_lookup(terms_hierarchy):
    """Simply convert the hierarchy to a lookup dictionary
    terms_hierarchy shall be deduplicated in terms of the term anchor/id (input contract)
    """

    terms_lookup = {}

    for term in terms_hierarchy:
        term_id = term["anchor"]
        assert term_id not in terms_lookup # Input
        terms_lookup[term_id] = term
        terms_lookup |= build_terms_lookup(term["children"])

    return terms_lookup


def extract_term_references(term):
    name_pattern_str = shared.terms.TERM_NAME_MULTILINE_PATTERN_STRING
    id_pattern_str = shared.terms.TERM_ID_PATTERN_STRING

    definition_text = "\n".join(term["definition"])
    term_id_pattern = re.compile(
        rf"\[(?:{name_pattern_str})\]\(#(?P<id>{id_pattern_str})\)")
    return [m.group("id") for m in term_id_pattern.finditer(definition_text)]


def extract_referenced_terms(term_ids, terms_lookup, seen_terms=None):
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
            extracted_terms.append(strip_children(term))
            seen_terms.add(term_id)
            referenced_terms = extract_term_references(term)
            _LOG.info(f"Terms referenced by the {term['anchor']} definition: {', '.join(referenced_terms)}")
            extracted_terms += extract_referenced_terms(
                referenced_terms, terms_lookup, seen_terms)

    return extracted_terms




def main(options):
    terms_hierarchy = []

    for input_path in options.input_paths:
        terms_hierarchy += process_input_path(options, input_path)

    # TODO: Filter out duplicates

    terms_lookup = build_terms_lookup(terms_hierarchy)
    terms_collection = extract_referenced_terms(options.term_ids, terms_lookup)

    if terms_collection:
        print(json.dumps(terms_collection, indent=2))
    else:
        print(json.dumps(terms_hierarchy, indent=2))

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
