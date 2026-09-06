# Copilot Instructions for gen_sandbox

## Project Overview

A C++20 genealogy/RDF data processing application (gen_sandbox) with modular architecture for working with person data, relationships, and genealogical queries. The project uses CMake for building, GoogleTest for testing, and integrates with RDF libraries (Redland, Raptor, Rasqal) for semantic data handling.

## Build, Test & Lint Commands

### Build
```bash
# Full build
cmake --build . 

# Or using make
make all
```

### Run Tests
```bash
# All tests
cmake --build . --target test
# Or: make test
# Or: /usr/bin/ctest

# Single test executable
cmake --build . --target gen_common_test && ./bin/gen_common_test
cmake --build . --target gen_person_test && ./bin/gen_person_test

# Single test case (using --gtest_filter)
./bin/gen_person_test --gtest_filter="TestClassName.TestName"
```

### Lint
```bash
# clang-tidy is integrated into CMake and runs during build
# It's configured in .clang-tidy (checks: modernize-*, performance-*, bugprone-*)
# To run explicitly:
clang-tidy code/person/src/main.cpp -- -I./code/person/include
```

## High-Level Architecture

### Module Structure
The codebase is organized into three main modules:

1. **common/** - Shared utilities and domain models
   - `person.hpp` - Person entity with notes, relationships, gender
   - `resource.hpp` - Base RDF resource wrapper for Redland library
   - `redland_utils.hpp` - RDF query and manipulation utilities
   - `note.hpp` - Genealogical notes attached to persons
   - `variable.hpp` - SPARQL variable abstraction
   - Utilities: `file_system_utils`, `string`, `command_line_utils`, `data_table`

2. **person/** - Person data management application
   - `option_parser.hpp` - CLI argument parsing (uses CLI11)
   - **command/** - Command implementations
     - `list` - List persons from RDF store
     - `details` - Show detailed person information
     - `deps` - Query person dependencies/relationships
     - `targets` - Query target persons (TODO/advanced queries)
   - **queries/** - SPARQL query implementations
     - `common.hpp` - Shared query builders
     - `deps.hpp` - Dependency/relationship queries
     - `details.hpp` - Person details queries
   - `error.hpp` - Person app exceptions

3. **sandbox/** - Experimental/test application (minimal)

### Key Dependencies
- **Redland RDF** - RDF data store and query engine (requires Raptor, Rasqal)
- **GoogleTest** - Unit testing framework (fetched via FetchContent)
- **CLI11** - Command-line argument parser (v2.4.2, fetched)
- **nlohmann/json** - JSON serialization (v3.11.3, fetched)
- **Tabulate** - Table output formatting (fetched, master branch)
- **spdlog** - Structured logging (system package)
- **Boost** - url library (gen_common), system library (gen_test_lib)

### Data Model Patterns
- **Person** inherits from **Resource** - Each person is an RDF resource with IRI identifier
- **Notes** - Metadata attached to persons (gender, relationships, descriptions)
- **Gender** enum - Male/Female/Unknown/Invalid with RDF URI mappings
- SPARQL queries return `librdf_statement` objects that are converted to domain models

## Key Conventions

### File Organization
- **include/** - Public headers for the module (mirrored from src/)
- **src/** - Implementation files (.cpp)
- **test/src/** - GoogleTest test implementations (parallel structure to src/)
- Headers use `#if !defined` include guards with uppercase module names (e.g., `COMMON_PERSON_HPP`)

### Naming Conventions
- Constants: `k_` prefix (e.g., `k_invalid_gender_note_id`, `k_person_type_uri`)
- Member variables: `_` suffix (e.g., `_person_id`)
- Functions: snake_case for utilities, PascalCase for class names
- RDF URIs: Use GedcomX vocabulary (e.g., `http://gedcomx.org/Person`, `http://gedcomx.org/Male`)

### Error Handling
- Custom exception hierarchy: `common_exception` (base) and `person_exception` (derived)
- Main catches all exception types with different exit codes:
  - 1: person_exception or common_exception
  - 2: std::exception
  - 3: unknown exceptions
- Use [[nodiscard]] attributes on functions returning important values

### Testing Patterns
- Tests use GoogleTest with GTest discovery
- Helper factory classes in test utilities (e.g., `comparable_note_factory.cpp`)
- Test library (gen_test_lib) provides assertion helpers and test utilities
- Assertions extended with custom matchers for domain objects

### CMake Patterns
- `set(CMAKE_CXX_CLANG_TIDY clang-tidy;)` - clang-tidy is enabled per-module
- Compile options: `-Wall -Wextra -Wpedantic` for GCC/Clang
- CMAKE_CXX_STANDARD = 20 (C++20 required)
- FetchContent used for external libraries (cli11, tabulate, json, googletest)
- system packages used for spdlog and Boost

### RDF/SPARQL Patterns
- Use Redland library's `librdf_query_execute()` for SPARQL
- Results are `librdf_stream` objects that yield statements
- Convert statements to domain objects (Person, Note, Variable)
- Query builders in `queries/common.cpp` construct SPARQL strings
- Always free Redland objects with appropriate librdf_*_free() calls

### Command Structure
- Each command inherits general structure: parse options → build query → execute → format output
- Output uses `tabulate` library for formatted tables
- Logging via spdlog with configurable levels (default: info)

### Code Quality
- Headers are linted by clang-tidy during build
- modernize-* checks enforce C++17/20 idioms
- performance-* and bugprone-* catches common issues
- Avoid swappable parameters (bugprone-easily-swappable-parameters is disabled)

### Documentation: Terms & Definitions

The `docs/sources/` directory contains knowledge source documentation organized as a Doxygen page hierarchy (visible under **Related Pages**).

**Directory structure:**
```
docs/sources/
├── index.md                          # Root "Sources" page ({#sources})
├── scenario-01_*.md                  # Scenario pages ({#sources_scenario_*})
├── terms-and-definitions.md          # Terms index page ({#sources_terms})
└── terms/
    ├── <term>.md                     # Top-level term page ({#term_id})
    └── <term>/
        ├── <sub-term>.md             # Sub-term page ({#sub_term_id})
        └── <sub-term>/
            └── <sub-sub-term>.md    # Deeper nesting as needed
```

**Doxygen page hierarchy conventions:**
- Each `.md` file is a Doxygen page; the page ID is set via `{#id}` on the first heading
- Root and scenario pages use the `sources_` prefix (e.g., `{#sources_terms}`, `{#sources_scenario_01}`) to avoid ID collisions
- Term pages use the term name directly in `snake_case` (e.g., `{#archive}`, `{#birth_event}`)
- Parent-child relationships are established using `\subpage child_id` in a `## Subtopics` section
- Each page links to its **direct children only** via `\subpage`; deeper descendants are handled by their own parent pages
- Inline cross-references within body text use standard markdown links: `[Term Name](#term_id)`

**File/ID naming:**
- File and directory names: `kebab-case` (e.g., `register-entry-annotation.md`)
- Doxygen page IDs: `snake_case` (e.g., `register_entry_annotation`)

**Term definition style:**
- The page heading is the term in Title Case followed by its Doxygen page ID: `# Term Name {#term_id}`
- The definition opens with the correct indefinite article ("A" or "An") followed by the term in bold (`__Term Name__`), then "is"
- Use present tense declarative sentences; state what the term *is*, not how it is used procedurally
- Keep definitions concise: one to three sentences; prefer one or two
- Cross-reference other terms inline using markdown links: `[Term Name](#term_id)`. Link only the **first occurrence** of each term within a page; use bold (`__Term Name__`) for subsequent occurrences of the same term.
- Clarifying examples are allowed inline, introduced with "such as", "including", or "e.g." in parentheses
- Do not reference specific child terms unnecessarily in the definition body — list them in `## Subtopics` instead
- End every definition sentence with a period
- `## Subtopics` lists direct children only via `\subpage child_id`, in alphabetical order
