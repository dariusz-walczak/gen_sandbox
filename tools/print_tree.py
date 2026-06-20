#!/usr/bin/env python3

import argparse
import enum
import logging
import sys

import shared.argparse_types
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

    return parser.parse_args(args)

def main(options):
    print(options.output_format.name)

if __name__ == '__main__':
    sys.exit(main(parse_options(sys.argv[1:])))
