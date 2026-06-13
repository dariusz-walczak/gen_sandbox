# Terms and Definitions — Authoring Conventions

## Term File Structure

- Terms are defined using markdown format.
- The term file name uses the kebab-case format and the `.md` extension, e.g.: `administrative-scope.md`
- A single term file describes only one term.
- The term name and anchor are defined by the anchor header.
- The anchor header is a first-level markdown header followed by the term name and the anchor:
    - Example: `# Term Name {#term_name}`
    - Python re pattern: `r"^# (?P<text>[\w \-().:,!?.]+) \{#(?P<anchor>[a-zA-Z0-9_]+)\}\s*$"`
- The term definition body starts with the first line following the anchor header.
- The term definition body ends with the first header following the anchor header (any header level)
- The leading and trailing blank lines are ignored
- All content preceeding the anchor header and following the closing header is a supplementary content.
- The supplementary content may provide extra clarifications, examples, todo notes, AI-instructions, etc.
- The terms hierarchy is reflected by the filesystem structure and a dedicated subterm section of the term file.
- The subterm section is preferred to be named "Subterms" (not enforced yet)
- The subterm section contains one or more doxygen subpage directives using the subterms anchors:
  `- \subpage subterm_name`

## Terms Filesystem Structure

- Each term that has subterms has its subterm directory
- The directory shares the parent directory with the term file
- The directory name is the term file name with only the `.md` extension stripped, e.g.: `administrative-scope.md` -> `administrative-scope`
- All subterm files and directories are created in the term directory

### Term File Esthetics

- When listed, terms are put in **alphabetical order**.
- A blank line separates the anchor heading from the definition body.
- A blank line separates the definition body from the closing header.

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

- Unresolved issues are tracked in a HTML comment block.
- The unresolved issues block is located at the top of the file, before the anchor header.
- Each issue has a short kebab-case slug and a plain-language description.
- Resolved issues are removed from the comment block.

## Conventions Reference

- Every `terms-and-definitions.md` file shall include the following comment at the top of the file, before the optional unresolved issues comment:
  `<!-- See docs/conventions/terms-and-definitions.md for authoring conventions. -->`
