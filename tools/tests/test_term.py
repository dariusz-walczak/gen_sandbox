import io

import pytest

import shared.term


@pytest.mark.parametrize("text,expected", [
    # Minimal valid term — anchor header only, no definition
    (
        "# Simple Term {#simple_term}\n",
        shared.term.Term(id="simple_term", title="Simple Term", path="<test>"),
    ),
    # Term with a single-line definition
    (
        "# My Term {#my_term}\n"
        "\n"
        "A __My Term__ is a test term.\n",
        shared.term.Term(
            id="my_term", title="My Term", path="<test>",
            definition=["A __My Term__ is a test term."]),
    ),
    # Term with a multi-line definition
    (
        "# Multi Term {#multi_term}\n"
        "\n"
        "First line.\n"
        "Second line.\n",
        shared.term.Term(
            id="multi_term", title="Multi Term", path="<test>",
            definition=["First line.", "Second line."]),
    ),
    # Lines before the anchor header are ignored
    (
        "<!-- comment -->\n"
        "# Prefixed Term {#prefixed_term}\n"
        "\n"
        "Definition.\n",
        shared.term.Term(
            id="prefixed_term", title="Prefixed Term", path="<test>",
            definition=["Definition."]),
    ),
    # Lines after the follow-up header are ignored
    (
        "# Follow Term {#follow_term}\n"
        "\n"
        "Definition.\n"
        "\n"
        "## Subtopics\n"
        "\n"
        "- \\subpage other_term\n",
        shared.term.Term(
            id="follow_term", title="Follow Term", path="<test>",
            definition=["Definition."]),
    ),
    # No anchor header — returns None
    (
        "No valid header here.\n",
        None,
    ),
    # Empty-lines-only definition
    (
        "# Strip Empty Lines {#strip_empty_lines}\n"
        "\n"
        "\n"
        "\n"
        "# Another section\n"
        "\n"
        "Добрий ранок\n",
        shared.term.Term(
            id="strip_empty_lines", title="Strip Empty Lines", path="<test>",
            definition=[]),
    ),
])
def test_load(text: str, expected: shared.term.Term | None) -> None:
    result = shared.term.load(io.StringIO(text), "<test>")
    assert result == expected
