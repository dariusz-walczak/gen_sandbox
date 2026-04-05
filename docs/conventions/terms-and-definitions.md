# Terms and Definitions — Authoring Conventions

## Structure

- Terms are listed in **alphabetical order**.
- Each term has a heading with a lowercase underscore-separated anchor:
  `## Term Name {#term_name}`
- A blank line separates the heading from the definition body.

## Definition Style

- Every definition follows the pattern: `A __Term__ is ...` (or `An __Term__ is ...`).
  Avoid "refers to", "describes", or other indirect openers.
- Definitions are written as a single sentence where possible.
- Inline examples use `(such as ...)` or `(e.g., ...)` parentheticals — no separate bullet lists.

## Bolding and Cross-References

- **Bold** (`__Term__`) is used **only** for terms defined elsewhere in this file.
- Terms not yet defined are written in plain text, even if they may be defined later.

## Scope and Precision

- Prefer narrow, specific definitions that reflect observed reality.
- Broaden a definition only when a concrete case requires it.

## Open Issues

- Unresolved issues are tracked in a single HTML comment block at the bottom of the file.
- Each issue has a short kebab-case slug and a plain-language description.
- Resolved issues are removed from the comment block.

## Conventions Reference

- Every `terms-and-definitions.md` file must include the following comment at the bottom:
  `<!-- See docs/conventions/terms-and-definitions.md for authoring conventions. -->`
