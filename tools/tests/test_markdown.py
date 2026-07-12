import pytest

import shared.markdown


@pytest.mark.parametrize("md_input,expected", [
    # Plain paragraph
    (
        "Hello world",
        "Hello world",
    ),
    # Strong / bold stripped
    (
        "Hello **bold** world",
        "Hello bold world",
    ),
    # Emphasis / italic stripped
    (
        "Hello *italic* world",
        "Hello italic world",
    ),
    # Inline code preserved as raw text
    (
        "Use `code` here",
        "Use code here",
    ),
    # Heading rendered as plain text line
    (
        "# Section Title",
        "Section Title",
    ),
    # Anchor link rendered as "caption {url}"
    (
        "[Term](#anchor)",
        "Term {anchor}",
    ),
    # Soft break in an anchor link caption should be correctly handled
    (
        "This is a [Link with Soft\nBreak in the Caption](#anchor)",
        "This is a Link with Soft Break in the Caption {anchor}",
    ),
    # Non-anchor-only url should be completely preserved
    (
        "This is [First Link](https://www.example.com) and this is"
        " [Second Link](https://www.example.com#should_work)",
        "This is First Link {https://www.example.com} and this is Second Link"
        " {https://www.example.com#should_work}",

    ),
    # Soft break inside paragraph collapsed to space
    (
        "Line one\nLine two",
        "Line one Line two",
    ),
    # Multiple paragraphs separated by blank line
    (
        "First paragraph\n\nSecond paragraph",
        "First paragraph\nSecond paragraph",
    ),
    # Unordered list items each on own line
    (
        "- Item A\n- Item B\n- Item C",
        "Item A\nItem B\nItem C",
    ),
    # Empty string
    (
        "",
        "",
    ),
])
def test_convert(md_input, expected):
    assert shared.markdown.genai_friendly_format(md_input) == expected
