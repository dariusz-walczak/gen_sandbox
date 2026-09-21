import argparse
import logging
import typing

import shared.error
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


def make_ranged_int(min_val: int, max_val: int) -> typing.Callable[[str], int]:
    if min_val > max_val:
        raise shared.error.AppError(
            shared.error.AppError.Codes.InputContractError,
            f"The min_val ({min_val}) mustn't be greater than the max_val ({max_val})")

    def _ranged_int(raw_val: str) -> int:
        try:
            int_val = int(raw_val)
        except ValueError:
            raise argparse.ArgumentTypeError(f"{repr(raw_val)} is not a valid integer")
        if int_val < min_val or int_val > max_val:
            raise argparse.ArgumentTypeError(
                f"{raw_val} is not from the [{min_val}, {max_val}] range")
        return int_val

    return _ranged_int


def logging_level(raw_val: str) -> str:
    normalized_str = raw_val.upper()

    if normalized_str not in logging.getLevelNamesMapping():
        raise argparse.ArgumentTypeError(
            f"{repr(raw_val)} is not a valid log level code name"
            f" (valid: {{{','.join(logging.getLevelNamesMapping())}}})")

    return normalized_str
