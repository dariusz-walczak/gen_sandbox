#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR=$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)
BUILD_DIR=${BUILD_DIR:-"$ROOT_DIR/build/coverage"}
COVERAGE_DIR=${COVERAGE_DIR:-"$ROOT_DIR/coverage"}

cmake -S "$ROOT_DIR" -B "$BUILD_DIR" -DCMAKE_BUILD_TYPE=Debug -DENABLE_CODE_COVERAGE=ON
cmake --build "$BUILD_DIR"

mkdir -p "$BUILD_DIR/code/person/test"
ln -sfn "$ROOT_DIR/code/person/test/data" "$BUILD_DIR/code/person/test/data"

ctest --test-dir "$BUILD_DIR" --output-on-failure

rm -rf "$COVERAGE_DIR"
mkdir -p "$COVERAGE_DIR/gen_common" "$COVERAGE_DIR/gen_person"

GCOVR_ARGS=(
  --root "$ROOT_DIR"
  --object-directory "$BUILD_DIR"
  --exclude '.*test/.*'
)

# Combined coverage for both libraries
GCOMBINED_FILTER='code/(common|person)/(src|include)/'
gcovr "${GCOVR_ARGS[@]}" \
  --filter "$GCOMBINED_FILTER" \
  --html-details -o "$COVERAGE_DIR/index.html" \
  --html-title "gen_sandbox libraries coverage" \
  --print-summary | tee "$COVERAGE_DIR/summary.txt"

# Coverage focused on the gen_common library
GCOMMON_FILTER='code/common/(src|include)/'
gcovr "${GCOVR_ARGS[@]}" \
  --filter "$GCOMMON_FILTER" \
  --html-details -o "$COVERAGE_DIR/gen_common/index.html" \
  --html-title "gen_common library coverage"

# Coverage focused on the gen_person_lib library
GPERSON_FILTER='code/person/(src|include)/'
gcovr "${GCOVR_ARGS[@]}" \
  --filter "$GPERSON_FILTER" \
  --html-details -o "$COVERAGE_DIR/gen_person/index.html" \
  --html-title "gen_person_lib coverage"

cat <<REPORT > "$COVERAGE_DIR/README.txt"
Coverage reports generated on $(date -u).

Open the following files in a browser:
  - coverage/index.html           (combined libraries)
  - coverage/gen_common/index.html (gen_common specific)
  - coverage/gen_person/index.html (gen_person_lib specific)

A textual summary is available in coverage/summary.txt.
REPORT

