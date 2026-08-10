import io
import os

import pytest

import shared.term

_DATA_ROOT = os.path.join(os.path.dirname(__file__), "data")


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
def test_load_from_file(text: str, expected: shared.term.Term | None) -> None:
    result = shared.term.load_from_file(io.StringIO(text), "<test>")
    assert result == expected

def _make_flat_term(term_id: str, title: str, definition_suffix: str) -> shared.term.Term:
    tests_dir = os.path.dirname(__file__)
    return shared.term.Term(
        id=term_id,
        title=title,
        path=os.path.join(tests_dir, "data", "term", "flat-term-list", f"{term_id}.md"),
        definition=[f"**{title}** is the capital of {definition_suffix}."])


@pytest.mark.parametrize("input_options,input_path,expected_seen_terms,expected_term_trees", [
    # The path locates a directory which is includes few non-term files only (it is not a term
    #  repo)
    (
        shared.term.Options(),
        "term/non-repo-dir",
        {},
        []
    ),
    # The path locates a directory with a flat list of term files (no sub-directories)
    (
        shared.term.Options(),
        "term/flat-term-list",
        {
            "abuja": shared.term.Term(
                id="abuja", title="Abuja",
                path=os.path.join(_DATA_ROOT, "term", "flat-term-list", "abuja.md"),
                definition=["**Abuja** is the capital of Nigeria."]),
            "accra": shared.term.Term(
                id="accra", title="Accra",
                path=os.path.join(_DATA_ROOT, "term", "flat-term-list", "accra.md"),
                definition=["**Accra** is the capital of Ghana."]),
            "addis_ababa": shared.term.Term(
                id="addis_ababa", title="Addis Ababa",
                path=os.path.join(_DATA_ROOT, "term", "flat-term-list", "addis_ababa.md"),
                definition=["**Addis Ababa** is the capital of Ethiopia."]),
            "algiers": shared.term.Term(
                id="algiers", title="Algiers",
                path=os.path.join(_DATA_ROOT, "term", "flat-term-list", "algiers.md"),
                definition=["**Algiers** is the capital of Algeria."]),
            "cairo": shared.term.Term(
                id="cairo", title="Cairo",
                path=os.path.join(_DATA_ROOT, "term", "flat-term-list", "cairo.md"),
                definition=["**Cairo** is the capital of Egypt."]),
            "nairobi": shared.term.Term(
                id="nairobi", title="Nairobi",
                path=os.path.join(_DATA_ROOT, "term", "flat-term-list", "nairobi.md"),
                definition=["**Nairobi** is the capital of Kenya."]),
            "pretoria": shared.term.Term(
                id="pretoria", title="Pretoria",
                path=os.path.join(_DATA_ROOT, "term", "flat-term-list", "pretoria.md"),
                definition=["**Pretoria** is the capital of South Africa."]),
        },
        [
            shared.term.Term(
                id="abuja", title="Abuja",
                path=os.path.join(_DATA_ROOT, "term", "flat-term-list", "abuja.md"),
                definition=["**Abuja** is the capital of Nigeria."]),
            shared.term.Term(
                id="accra", title="Accra",
                path=os.path.join(_DATA_ROOT, "term", "flat-term-list", "accra.md"),
                definition=["**Accra** is the capital of Ghana."]),
            shared.term.Term(
                id="addis_ababa", title="Addis Ababa",
                path=os.path.join(_DATA_ROOT, "term", "flat-term-list", "addis_ababa.md"),
                definition=["**Addis Ababa** is the capital of Ethiopia."]),
            shared.term.Term(
                id="algiers", title="Algiers",
                path=os.path.join(_DATA_ROOT, "term", "flat-term-list", "algiers.md"),
                definition=["**Algiers** is the capital of Algeria."]),
            shared.term.Term(
                id="cairo", title="Cairo",
                path=os.path.join(_DATA_ROOT, "term", "flat-term-list", "cairo.md"),
                definition=["**Cairo** is the capital of Egypt."]),
            shared.term.Term(
                id="nairobi", title="Nairobi",
                path=os.path.join(_DATA_ROOT, "term", "flat-term-list", "nairobi.md"),
                definition=["**Nairobi** is the capital of Kenya."]),
            shared.term.Term(
                id="pretoria", title="Pretoria",
                path=os.path.join(_DATA_ROOT, "term", "flat-term-list", "pretoria.md"),
                definition=["**Pretoria** is the capital of South Africa."]),
        ]
    ),
])
def test_process_input_path_normal_cases(
        input_options: shared.term.Options,
        input_path: str,
        expected_seen_terms: dict[str, shared.term.Term],
        expected_term_trees: list[shared.term.Term]) -> None:

    input_term_path = os.path.join(_DATA_ROOT, input_path)
    output_seen_terms: dict[str, shared.term.Term] = {}

    output_term_trees = shared.term.process_input_path(
        input_options, output_seen_terms, input_term_path)

    assert output_seen_terms == expected_seen_terms
    assert sorted(output_term_trees, key=lambda t: t.id) == \
        sorted(expected_term_trees, key=lambda t: t.id)
