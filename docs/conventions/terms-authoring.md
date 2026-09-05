# Terms Authoring Conventions {#terms_authoring_conventions}

## Term File Structure

- Terms are defined using markdown format.
- The term file name uses the kebab-case format and the `.md` extension, e.g.: `administrative-scope.md`
- A single term file describes only one term.
- The term name and anchor are defined by the anchor header.
- The anchor header must be the first line in the term file for doxygen to represent the term tree correctly.
- The anchor header is a first-level markdown header followed by the term name and the anchor:
    - Example: `# Term Name {#term_name}`
    - Python re pattern: `r"^# (?P<text>[\w \-().:,!?.]+) \{#(?P<anchor>[a-zA-Z0-9_]+)\}\s*$"`
- The closing header is the first header of any level following the anchor header.
- The term definition body starts with the first line following the anchor header.
- The term definition body ends with the closing header.
- The leading and trailing blank lines are ignored
- All content following the closing header is supplementary content.
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

- Every definition follows the pattern: `A __Term__ is ...` (or `An __Alternative Term__ is ...`).
  Avoid "refers to", "describes", or other indirect openers.
- Definitions are written as a single sentence where possible.
- Inline examples use `(such as ...)` or `(e.g., ...)` parentheticals — no separate bullet lists.

## Bolding and Cross-References

- **Bold** (`__Term__`) is used for each occurrence of the defined term name.
- **Bold** is used for terms not yet defined (e.g.: `A __Term__ is a kind of __Something__`)

## Scope and Precision

- Prefer narrow, specific definitions that reflect observed reality.
- Broaden a definition only when a concrete case requires it.

## Open Issues

- Unresolved issues are tracked in the `## Open Issues` section.
- The `## Open Issues` section is located in the supplementary part of the term file.
- Each issue has a short kebab-case slug and a plain-language description.
- Resolved issues are removed from the `## Open Issues` section.
