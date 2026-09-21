import argparse
import pytest
import re

import shared.argparse_types
import shared.error


def test_ranged_int_input_contract_violation() -> None:
    msg_pattern = re.compile(r"The min_val \(\d+\) mustn't be greater than the max_val \(\d+\)")
    code_check = lambda e: e.code == shared.error.AppError.Codes.InputContractError

    with pytest.raises(shared.error.AppError, match=msg_pattern, check=code_check):
        shared.argparse_types.make_ranged_int(5, 4)


@pytest.mark.parametrize("input_value", ["0x12", "wrong", "True", "", "10.01"])
def test_ranged_int_invalid_integer(input_value: str) -> None:
    msg_pattern = "is not a valid integer"

    ranged_int_cb = shared.argparse_types.make_ranged_int(0, 256)

    with pytest.raises(argparse.ArgumentTypeError, match=msg_pattern):
        ranged_int_cb(input_value)


@pytest.mark.parametrize(
    "input_min_val,input_max_val,input_raw_val,expected_val",
    [
        (2, 5, "-1", None),
        (2, 5, "0", None),
        (2, 5, "1", None),
        (2, 5, "2", 2),
        (2, 5, "3", 3),
        (2, 5, "4", 4),
        (2, 5, "5", 5),
        (2, 5, "6", None),
        (2, 5, "10000", None),
        (7, 7, "1", None),
        (7, 7, "6", None),
        (7, 7, "7", 7),
        (7, 7, "8", None),
        (7, 7, "1234", None),
    ]
)
def test_ranged_int_valid_integer(
        input_min_val: int,
        input_max_val: int,
        input_raw_val: str,
        expected_val: int | None) -> None:

    msg = fr"{input_raw_val} is not from the \[{input_min_val}, {input_max_val}\] range"
    ranged_int_cb = shared.argparse_types.make_ranged_int(input_min_val, input_max_val)

    if expected_val is None:
        with pytest.raises(argparse.ArgumentTypeError, match=msg):
            ranged_int_cb(input_raw_val)
    else:
        assert expected_val == ranged_int_cb(input_raw_val)
