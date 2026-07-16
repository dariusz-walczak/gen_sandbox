import re

import mistune

import shared.terms


class PlainTextRenderer(mistune.BaseRenderer):
    NAME = "plain"

    def text(self, token, state):
        return token["raw"]

    def softbreak(self, token, state):
        # Single newline inside a paragraph → collapse to space
        return " "

    def linebreak(self, token, state):
        # Hard line break (two trailing spaces + newline) → space
        return " "

    def paragraph(self, token, state):
        text = self.render_tokens(token["children"], state).strip()
        return text + "\n"

    def heading(self, token, state):
        text = self.render_tokens(token["children"], state).strip()
        return text + "\n"

    def list(self, token, state):
        return self.render_tokens(token["children"], state)

    def list_item(self, token, state):
        text = self.render_tokens(token["children"], state).strip()
        return text + "\n"

    def block_text(self, token, state):
        return self.render_tokens(token["children"], state)

    def blank_line(self, token, state):
        return ""

    def thematic_break(self, token, state):
        return "\n"

    def block_code(self, token, state):
        return token["raw"] + "\n"

    def codespan(self, token, state):
        return token["raw"]

    def strong(self, token, state):
        return self.render_tokens(token["children"], state)

    def emphasis(self, token, state):
        return self.render_tokens(token["children"], state)

    def link(self, token, state):
        # Custom reference format: [caption](#anchor) → caption {anchor}

        caption = self.render_tokens(token["children"], state)
        url = token["attrs"].get("url", "")

        term_id_match = re.match(rf"^#(?P<id>{shared.terms.TERM_ID_PATTERN_STRING})$", url.strip())
        if term_id_match is not None:
            return f"{caption} {{{term_id_match.group('id')}}}"
        else:
            return f"{caption} {{{url}}}"

    def image(self, token, state):
        return token["attrs"].get("alt", "")

    def raw(self, token, state):
        return token.get("raw", "")


# Convert the text to the GenAI friendly format:
# * References: [Caption](#anchor) -> Caption {anchor}
# * Strip emphasis and strong
# * Eliminate soft breaks
# * Eliminate the last trailing newline
def genai_friendly_format(md_text: str) -> str:
    md_instance = mistune.create_markdown(renderer=PlainTextRenderer())
    # The strip call is supposed to eliminate the trailing newline
    return md_instance(md_text).strip()
