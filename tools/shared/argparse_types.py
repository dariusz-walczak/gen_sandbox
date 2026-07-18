import argparse
import logging

import shared.output


def format_enum(raw_val: str) -> shared.output.Format:
    try:
        enum_val = shared.output.Format[raw_val]
    except KeyError:
        raise argparse.ArgumentTypeError(
            f"{repr(raw_val)} is not a valid format code name"
            f" (valid: {{{','.join([v.name for v in shared.output.Format])}}})")
    return enum_val


def positive_int(raw_val: str) -> int:
    try:
        int_val = int(raw_val)
    except ValueError as e:
        raise argparse.ArgumentTypeError(f"{repr(raw_val)} is not a valid integer ({e})")
    if int_val <= 0:
        raise argparse.ArgumentTypeError(f"{raw_val} is not a positive integer")
    return int_val


def logging_level(raw_val: str) -> str:
    normalized_str = raw_val.upper()

    if normalized_str not in logging.getLevelNamesMapping():
        raise argparse.ArgumentTypeError(
            f"{repr(raw_val)} is not a valid log level code name"
            f" (valid: {{{','.join(logging.getLevelNamesMapping())}}})")

    return normalized_str
