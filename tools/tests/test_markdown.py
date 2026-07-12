import pytest

from shared.markdown import convert


@pytest.mark.parametrize("md_input,expected", [
    # Plain paragraph
    (
        "Hello world",
        "Hello world\n",
    ),
    # Strong / bold stripped
    (
        "Hello **bold** world",
        "Hello bold world\n",
    ),
    # Emphasis / italic stripped
    (
        "Hello *italic* world",
        "Hello italic world\n",
    ),
    # Inline code preserved as raw text
    (
        "Use `code` here",
        "Use code here\n",
    ),
    # Heading rendered as plain text line
    (
        "# Section Title",
        "Section Title\n",
    ),
    # Anchor link rendered as "caption {url}"
    (
        "[Term](#anchor)",
        "Term {anchor}\n",
    ),
    # Soft break in an anchor link caption should be correctly handled
    (
        "This is a [Link with Soft\nBreak in the Caption](#anchor)",
        "This is a Link with Soft Break in the Caption {anchor}\n",
    ),
    # Non-anchor-only url should be completely preserved
    (
        "This is [First Link](https://www.example.com) and this is"
        " [Second Link](https://www.example.com#should_work)",
        "This is First Link {https://www.example.com} and this is Second Link"
        " {https://www.example.com#should_work}\n",

    ),
    # Soft break inside paragraph collapsed to space
    (
        "Line one\nLine two",
        "Line one Line two\n",
    ),
    # Multiple paragraphs separated by blank line
    (
        "First paragraph\n\nSecond paragraph",
        "First paragraph\nSecond paragraph\n",
    ),
    # Unordered list items each on own line
    (
        "- Item A\n- Item B\n- Item C",
        "Item A\nItem B\nItem C\n",
    ),
    # Empty string
    (
        "",
        "",
    ),
])
def test_convert(md_input, expected):
    assert convert(md_input) == expected
