import re
import typing

import mistune

import shared.error
import shared.terms


class PlainTextRenderer(mistune.BaseRenderer):
    NAME = "plain"

    def text(self, token: dict[str, typing.Any], state: mistune.BlockState) -> str:
        return token["raw"]  # type: ignore[no-any-return]

    def softbreak(self, token: dict[str, typing.Any], state: mistune.BlockState) -> str:
        # Single newline inside a paragraph → collapse to space
        return " "

    def linebreak(self, token: dict[str, typing.Any], state: mistune.BlockState) -> str:
        # Hard line break (two trailing spaces + newline) → space
        return " "

    def paragraph(self, token: dict[str, typing.Any], state: mistune.BlockState) -> str:
        text = self.render_tokens(token["children"], state).strip()
        return text + "\n"

    def heading(self, token: dict[str, typing.Any], state: mistune.BlockState) -> str:
        text = self.render_tokens(token["children"], state).strip()
        return text + "\n"

    def list(self, token: dict[str, typing.Any], state: mistune.BlockState) -> str:
        return self.render_tokens(token["children"], state)

    def list_item(self, token: dict[str, typing.Any], state: mistune.BlockState) -> str:
        text = self.render_tokens(token["children"], state).strip()
        return text + "\n"

    def block_text(self, token: dict[str, typing.Any], state: mistune.BlockState) -> str:
        return self.render_tokens(token["children"], state)

    def blank_line(self, token: dict[str, typing.Any], state: mistune.BlockState) -> str:
        return ""

    def thematic_break(self, token: dict[str, typing.Any], state: mistune.BlockState) -> str:
        return "\n"

    def block_code(self, token: dict[str, typing.Any], state: mistune.BlockState) -> str:
        return token["raw"] + "\n"  # type: ignore[no-any-return]

    def codespan(self, token: dict[str, typing.Any], state: mistune.BlockState) -> str:
        return token["raw"]  # type: ignore[no-any-return]

    def strong(self, token: dict[str, typing.Any], state: mistune.BlockState) -> str:
        return self.render_tokens(token["children"], state)

    def emphasis(self, token: dict[str, typing.Any], state: mistune.BlockState) -> str:
        return self.render_tokens(token["children"], state)

    def link(self, token: dict[str, typing.Any], state: mistune.BlockState) -> str:
        # Custom reference format: [caption](#anchor) → caption {anchor}

        caption = self.render_tokens(token["children"], state)
        url: str = token["attrs"].get("url", "")

        term_id_match = re.match(rf"^#(?P<id>{shared.terms.TERM_ID_PATTERN_STRING})$", url.strip())
        if term_id_match is not None:
            return f"{caption} {{{term_id_match.group('id')}}}"
        else:
            return f"{caption} {{{url}}}"

    def image(self, token: dict[str, typing.Any], state: mistune.BlockState) -> str:
        return token["attrs"].get("alt", "")  # type: ignore[no-any-return]

    def raw(self, token: dict[str, typing.Any], state: mistune.BlockState) -> str:
        return token.get("raw", "")  # type: ignore[no-any-return]


# Convert the text to the GenAI friendly format:
# * References: [Caption](#anchor) -> Caption {anchor}
# * Strip emphasis and strong
# * Eliminate soft breaks
# * Eliminate the last trailing newline
def genai_friendly_format(md_text: str) -> str:
    md_instance = mistune.create_markdown(renderer=PlainTextRenderer())
    md_result = md_instance(md_text)
    # Mistune returns `str` when a renderer is provided, or `list[dict]` in Abstract Syntax Tree
    #  (AST) mode, which is triggered when the renderer is `None`. The renderer is always provided
    #  during the markdown instance creation above, so the call result is always str.
    if not isinstance(md_result, str):
        raise shared.error.AppError(
            shared.error.AppError.Codes.InternalError,
            f"The markdown instance call didn't return `str` as expected (got {type(md_result)}"
            " instead)")
    # Strip to eliminate the trailing newline added by paragraph/heading renderers
    return md_result.strip()
