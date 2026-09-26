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

CXX_COMPILER_PATH=$(cache_lookup "CMAKE_CXX_COMPILER:FILEPATH")

if [[ -z "$CXX_COMPILER_PATH" ]]; then
  echo "Unable to determine the configured C++ compiler from $CMAKE_CACHE" >&2
  exit 1
fi

# The compiler ID and version are not recorded in the CMake cache; query the
# compiler itself. Resolve symlinks so that alternatives-based wrappers
# (e.g. /usr/bin/c++ -> g++-14) point at the real toolchain binary.
CXX_COMPILER_REAL=$(readlink -f "$CXX_COMPILER_PATH")
COMPILER_VERSION_OUTPUT=$("$CXX_COMPILER_REAL" --version)

CXX_COMPILER_ID=GNU
if grep -qi 'clang version' <<<"$COMPILER_VERSION_OUTPUT"; then
  CXX_COMPILER_ID=Clang
fi

find_executable() {
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

GCOV_EXECUTABLE=(gcov)
if [[ "$CXX_COMPILER_ID" == "Clang" ]]; then
  CXX_COMPILER_VERSION=$(head -n1 <<<"$COMPILER_VERSION_OUTPUT" | grep -oE '[0-9]+(\.[0-9]+)+' | head -n1)

  llvm_cov_candidates=()

  compiler_dir=$(dirname "$CXX_COMPILER_REAL")
  llvm_cov_candidates+=("$compiler_dir/llvm-cov")

  compiler_basename=$(basename "$CXX_COMPILER_REAL")
  if [[ "$compiler_basename" == clang++-* ]]; then
    suffix=${compiler_basename#clang++-}
    llvm_cov_candidates+=("$compiler_dir/llvm-cov-$suffix")
  fi

  if [[ -d "$compiler_dir" ]]; then
    toolchain_root=$(dirname "$compiler_dir")
    llvm_cov_candidates+=("$toolchain_root/bin/llvm-cov")
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

  if find_executable llvm_cov_path "${llvm_cov_candidates[@]}"; then
    GCOV_EXECUTABLE=("$llvm_cov_path" gcov)
  else
    {
      echo "Failed to locate a suitable llvm-cov executable needed to process clang coverage data."
      echo "Checked the following candidates:"
      printf '  - %s\n' "${llvm_cov_candidates[@]}"
    } >&2
    exit 1
  fi
else
  CXX_COMPILER_VERSION=$("$CXX_COMPILER_REAL" -dumpfullversion 2>/dev/null || "$CXX_COMPILER_REAL" -dumpversion)
  IFS=. read -r version_major _ <<<"$CXX_COMPILER_VERSION"
  compiler_dir=$(dirname "$CXX_COMPILER_REAL")

  # The gcov tool version must match the compiler that produced the coverage
  # data. Plain `gcov` may point at a different major version than the
  # configured compiler (e.g. GCC 14 with the distro default gcov 13), which
  # makes gcov fail on the recorded data.
  gcov_candidates=("$compiler_dir/gcov-$version_major" "gcov-$version_major")

  if find_executable gcov_path "${gcov_candidates[@]}"; then
    GCOV_EXECUTABLE=("$gcov_path")
  elif command -v gcov >/dev/null 2>&1 \
      && [[ "$(gcov --version | grep -oE '[0-9]+(\.[0-9]+)+' | head -n1 | cut -d. -f1)" == "$version_major" ]]; then
    GCOV_EXECUTABLE=(gcov)
  else
    {
      echo "Failed to locate a gcov executable matching the $CXX_COMPILER_VERSION compiler."
      echo "Checked the following candidates:"
      printf '  - %s\n' "${gcov_candidates[@]}"
      echo "  - gcov (system default)"
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
