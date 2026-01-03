#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR=$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)
BUILD_DIR=${BUILD_DIR:-"$ROOT_DIR/build/coverage"}
COVERAGE_DIR=${COVERAGE_DIR:-"$ROOT_DIR/coverage"}

cmake -S "$ROOT_DIR" -B "$BUILD_DIR" -DCMAKE_BUILD_TYPE=Debug -DENABLE_CODE_COVERAGE=ON

detect_parallel_jobs() {
  local cores=""
  if command -v nproc >/dev/null 2>&1; then
    cores=$(nproc)
  elif command -v getconf >/dev/null 2>&1; then
    cores=$(getconf _NPROCESSORS_ONLN)
  fi

  if [[ -z "$cores" ]] || ! [[ "$cores" =~ ^[0-9]+$ ]] || (( cores < 1 )); then
    cores=1
  fi

  if (( cores <= 2 )); then
    echo 1
  else
    echo $((cores - 2))
  fi
}

BUILD_JOBS=$(detect_parallel_jobs)
cmake --build "$BUILD_DIR" --parallel "$BUILD_JOBS"

CMAKE_CACHE="$BUILD_DIR/CMakeCache.txt"
if [[ ! -f "$CMAKE_CACHE" ]]; then
  echo "Failed to locate CMake cache at $CMAKE_CACHE" >&2
  exit 1
fi

cache_lookup() {
  local key="$1"
  awk -F= -v needle="$key" '$1 == needle {print $2; exit}' "$CMAKE_CACHE"
}

CXX_COMPILER_ID=$(cache_lookup "CMAKE_CXX_COMPILER_ID:STRING")
CXX_COMPILER_PATH=$(cache_lookup "CMAKE_CXX_COMPILER:FILEPATH")
CXX_COMPILER_VERSION=$(cache_lookup "CMAKE_CXX_COMPILER_VERSION:STRING")

if [[ -z "$CXX_COMPILER_ID" ]]; then
  echo "Unable to determine the configured C++ compiler from $CMAKE_CACHE" >&2
  exit 1
fi

GCOV_EXECUTABLE=(gcov)
if [[ "$CXX_COMPILER_ID" == "Clang" ]]; then
  find_llvm_cov() {
    local -n _result=$1
    shift
    local candidate
    for candidate in "$@"; do
      [[ -z "$candidate" ]] && continue

      if [[ "$candidate" == */* ]]; then
        if [[ -x "$candidate" ]]; then
          _result="$candidate"
          return 0
        fi
      elif command -v "$candidate" >/dev/null 2>&1; then
        _result=$(command -v "$candidate")
        return 0
      fi
    done
    return 1
  }

  llvm_cov_candidates=()

  if [[ -n "$CXX_COMPILER_PATH" ]]; then
    compiler_dir=$(dirname "$CXX_COMPILER_PATH")
    llvm_cov_candidates+=("$compiler_dir/llvm-cov")

    compiler_basename=$(basename "$CXX_COMPILER_PATH")
    if [[ "$compiler_basename" == clang++-* ]]; then
      suffix=${compiler_basename#clang++-}
      llvm_cov_candidates+=("$compiler_dir/llvm-cov-$suffix")
    fi

    if [[ -d "$compiler_dir" ]]; then
      toolchain_root=$(dirname "$compiler_dir")
      llvm_cov_candidates+=("$toolchain_root/bin/llvm-cov")
    fi
  fi

  if [[ -n "$CXX_COMPILER_VERSION" ]]; then
    IFS=. read -r version_major version_minor _ <<<"$CXX_COMPILER_VERSION"
    if [[ -n "$version_major" ]]; then
      llvm_cov_candidates+=("llvm-cov-$version_major" "llvm-cov$version_major")
      if [[ -n "$version_minor" ]]; then
        llvm_cov_candidates+=("llvm-cov-$version_major.$version_minor")
      fi
    fi
  fi

  llvm_cov_candidates+=("llvm-cov")

  if find_llvm_cov llvm_cov_path "${llvm_cov_candidates[@]}"; then
    GCOV_EXECUTABLE=("$llvm_cov_path" gcov)
  else
    {
      echo "Failed to locate a suitable llvm-cov executable needed to process clang coverage data."
      echo "Checked the following candidates:"
      printf '  - %s\n' "${llvm_cov_candidates[@]}"
    } >&2
    exit 1
  fi
fi

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

GCOVR_ARGS+=(--gcov-executable "${GCOV_EXECUTABLE[@]}")

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
