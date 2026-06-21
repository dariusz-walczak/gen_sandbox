import argparse
import logging

import shared.output


def format_enum(str_val):
    try:
        enum_val = shared.output.Format[str_val]
    except KeyError:
        raise argparse.ArgumentTypeError(
            f"{repr(str_val)} is not a valid format code name"
            f" (valid: {{{','.join([v.name for v in shared.output.Format])}}})")
    return enum_val


def positive_int(str_val):
    try:
        int_val = int(str_val)
    except ValueError as e:
        raise argparse.ArgumentTypeError(f"{repr(str_val)} is not a valid integer ({e})")
    if int_val <= 0:
        raise argparse.ArgumentTypeError(f"{str_val} is not a positive integer")
    return int_val


def logging_level(str_val):
    normalized_str = str_val.upper()

    if normalized_str not in logging.getLevelNamesMapping():
        raise argparse.ArgumentTypeError(
            f"{repr(str_val)} is not a valid log level code name"
            f" (valid: {{{','.join(logging.getLevelNamesMapping())}}})")

    return normalized_str
