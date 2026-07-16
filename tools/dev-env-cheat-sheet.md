# Dev Environment Cheat Sheet

## Install / Update Dependencies

```bash
# Install all dependencies (including dev)
uv sync --dev

# Update all dependencies to latest allowed versions
uv lock --upgrade && uv sync --dev
```

## Development Tools

### Ruff — Linter & Formatter

```bash
# Check for lint issues
uv run ruff check .

# Fix auto-fixable issues
uv run ruff check --fix .

# Format code
uv run ruff format .

# Check formatting without modifying files
uv run ruff format --check .
```

### Mypy — Static Type Checker

```bash
uv run mypy .
```

### Pytest — Test Runner

```bash
# Run all tests (with coverage report)
uv run pytest

# Run a specific test file
uv run pytest tests/test_markdown.py

# Run a specific test by name
uv run pytest tests/test_markdown.py::test_name
```
