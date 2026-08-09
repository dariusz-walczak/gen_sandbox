import logging
import pydantic
import re
import typing

# NOTE: Keep the TERM_NAME_SINGLELINE_PATTERN_STRING and the TERM_NAME_MULTILINE_PATTERN_STRING
#       constants consistent in terms of all non-whitespace characters.
#       When the pattern changes, update it copy in the docs/conventions/terms-and-definitions.md
#       document (search for 'Python re pattern'):
TERM_NAME_SINGLELINE_PATTERN_STRING = r"[\w \-().:,!?.]+"
TERM_NAME_MULTILINE_PATTERN_STRING = r"[\w\s\-().:,!?.]+"
TERM_ID_PATTERN_STRING = r"[a-zA-Z0-9_]+"

_LOG = logging.getLogger(__name__)


class Term(pydantic.BaseModel):
    id: str
    title: str | None = None
    path: str
    definition: list[str] = []
    children: list["Term"] = []


def load(term_file: typing.IO[str], term_path: str) -> Term | None:
    # [ignored lines]
    # # Valid Anchor Header {#valid_anchor_header}
    # [definition lines]
    # # Follow-up Header
    # [ignored lines]

    basic_header_pattern = re.compile(r"^#+ .+$")

    name_pattern_str = TERM_NAME_SINGLELINE_PATTERN_STRING
    id_pattern_str = TERM_ID_PATTERN_STRING

    anchor_header_pattern = re.compile(
        rf"^# (?P<name>{name_pattern_str}) \{{#(?P<id>{id_pattern_str})\}}\s*$")

    _LOG.debug(f"Reading the term file ({term_path})")

    term = None

    for line in term_file:
        line = line.rstrip("\n")

        if term is None:
            _LOG.debug("The valid anchor header wasn't yet encountered")

            m = anchor_header_pattern.match(line)

            if m is not None:
                _LOG.debug("The valid anchor header was just encountered")

                term = Term(id=m.group("id"), title=m.group("name"), path=term_path)
        elif basic_header_pattern.match(line):
            _LOG.debug("The follow-up header was encountered")
            # The definition line list is complete. All remaining lines are ignored.
            break
        else:
            _LOG.debug("A definition line was encountered")
            # A line between the valid anchor header and the first follow-up header
            term.definition.append(line)

    if term is not None:
        term.definition = strip_empty_lines(term.definition)

    return term


def strip_empty_lines(lines: list[str]) -> list[str]:
    """Remove empty lines from the front and the end of the provided line list"""
    if not lines:
        return []

    front_non_empty_offset = next((i for i, s in enumerate(lines) if s), None)
    if front_non_empty_offset is None:
        return []

    back_non_empty_offset = next((i for i, s in enumerate(reversed(lines)) if s), None)
    assert back_non_empty_offset is not None

    return lines[front_non_empty_offset : len(lines) - back_non_empty_offset]
