import pydantic

# NOTE: Keep the TERM_NAME_SINGLELINE_PATTERN_STRING and the TERM_NAME_MULTILINE_PATTERN_STRING
#       constants consistent in terms of all non-whitespace characters.
#       When the pattern changes, update it copy in the docs/conventions/terms-and-definitions.md
#       document (search for 'Python re pattern'):
TERM_NAME_SINGLELINE_PATTERN_STRING = r"[\w \-().:,!?.]+"
TERM_NAME_MULTILINE_PATTERN_STRING = r"[\w\s\-().:,!?.]+"
TERM_ID_PATTERN_STRING = r"[a-zA-Z0-9_]+"


class Term(pydantic.BaseModel):
    id: str
    title: str | None = None
    path: str
    definition: list[str] = []
    children: list["Term"] = []
