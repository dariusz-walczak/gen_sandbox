#!/usr/bin/env python3

import argparse
import enum
import logging
import sys

logging.basicConfig(
    level=logging.DEBUG,
    format="[%(levelname)s] %(message)s",
    stream=sys.stderr)

_LOG = logging.getLogger()

class Format(enum.Enum):
    HUMAN = enum.auto()
    MACHINE = enum.auto()
    SYMBOLIC = enum.auto()

    def __str__(self):
        return self.name


def parse_options(args):
    def _format_enum(str_val):
        try:
            enum_val = Format[str_val]
        except KeyError:
            raise argparse.ArgumentTypeError(f"{repr(str_val)} is not a valid format code name (allowed: {[v.name for v in Format]}")
        return enum_val

    def _positive_int(str_val):
        try:
            int_val = int(str_val)
        except ValueError as e:
            raise argparse.ArgumentTypeError(f"{repr(str_val)} is not a valid integer ({e})")
        if int_val <= 0:
            raise argparse.ArgumentTypeError(f"{str_val} is not a positive integer")
        return int_val

    parser = argparse.ArgumentParser()
    default_format = Format.HUMAN
    parser.add_argument(
        "-f", "--format", dest="format", choices=[x for x in Format], type=_format_enum,
        default=default_format,
        help=f"Output format (default: {default_format.name})")

    return parser.parse_args(args)

def main(options):
    print(options.format.name)

if __name__ == '__main__':
    sys.exit(main(parse_options(sys.argv[1:])))
