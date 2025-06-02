#! /bin/bash
#
# Test script for geda-update-databases.sh
# It should exit with 0 on success, and a non-zero value on failure.

# Test script for geda-update-databases.sh
SCRIPT_TO_TEST="../geda-update-databases.sh"
TEST_DIR="test_workdir"
LOG_FILE="test_output.log"
FAIL_COUNT=0

test_usage() {
  echo "Usage: $0 [--help]"
  echo ""
  echo "Runs tests for geda-update-databases.sh."
  exit 0
}

if [ "$1" = "--help" ]; then
  test_usage
fi

echo "Running tests for geda-update-databases.sh..."

cleanup() {
  echo "Cleaning up..."
  rm -rf "${TEST_DIR}" "${LOG_FILE}"
}

setup_test_env() {
  echo "Setting up test environment..."
  cleanup
  mkdir -p "${TEST_DIR}/share/mime/packages"
  mkdir -p "${TEST_DIR}/share/applications"
  # Create a dummy SystemVersion.plist to simulate macOS if needed for a test
  # mkdir -p "${TEST_DIR}/System/Library/CoreServices/"
  # touch "${TEST_DIR}/System/Library/CoreServices/SystemVersion.plist" # No longer needed for OS-agnostic script
}

# Helper to run a test and check its exit code and output
# Usage: run_test <description> <expected_exit_code> <grep_pattern_for_success> <script_args...>
run_test() {
  DESC="$1"
  EXPECTED_EC="$2"
  GREP_PATTERN="$3"
  shift 3
  ARGS="$@"

  echo "-----------------------------------------------------"
  echo "RUNNING TEST: ${DESC}"
  echo "COMMAND: ${SCRIPT_TO_TEST} ${ARGS}"
  # Override OS detection for specific tests if necessary by manipulating environment
  # e.g., export FAKE_OS_TYPE="macos" and modify geda-update-databases.sh to check FAKE_OS_TYPE
  # For now, we rely on the script's internal detection or test on the actual host OS.

  "${SCRIPT_TO_TEST}" ${ARGS} > "${LOG_FILE}" 2>&1
  EC=$?

  if [ "${EC}" -ne "${EXPECTED_EC}" ]; then
    echo "FAIL: ${DESC} - Expected exit code ${EXPECTED_EC}, got ${EC}"
    cat "${LOG_FILE}"
    FAIL_COUNT=$((FAIL_COUNT + 1))
    return 1
  fi

  if ! grep -q "${GREP_PATTERN}" "${LOG_FILE}"; then
    echo "FAIL: ${DESC} - Expected output pattern '${GREP_PATTERN}' not found."
    cat "${LOG_FILE}"
    FAIL_COUNT=$((FAIL_COUNT + 1))
    return 1
  fi

  echo "PASS: ${DESC}"
  return 0
}

# --- Test Cases ---

setup_test_env

# Test 0: Main script --help
run_test "Main script --help" 1 "Usage: ../geda-update-databases.sh TYPE TARGET_DIR" --help


# Test 1: MIME update with a dummy XML file
mkdir -p "${TEST_DIR}/share/geda-ai/mime/packages"
mkdir -p "${TEST_DIR}/share/geda-ai/db"
touch "${TEST_DIR}/share/geda-ai/mime/packages/geda-test.xml"
run_test "MIME update (with file)" 0 "gEDA-AI internal MIME database updated" mime "${TEST_DIR}/share/geda-ai/mime/packages"
if [ ! -f "${TEST_DIR}/share/geda-ai/db/mime.cache" ]; then
    echo "FAIL: MIME update (with file) - Cache file ${TEST_DIR}/share/geda-ai/db/mime.cache not created."
    cat "${LOG_FILE}"
    FAIL_COUNT=$((FAIL_COUNT + 1))
fi

# Test 2: MIME update with no XML files
rm -f "${TEST_DIR}/share/geda-ai/mime/packages/geda-test.xml" # Ensure no files
run_test "MIME update (no files)" 0 "No gEDA-AI MIME XML files found" mime "${TEST_DIR}/share/geda-ai/mime/packages"


# Test 3: Desktop update with a dummy .desktop file
mkdir -p "${TEST_DIR}/share/geda-ai/applications"
touch "${TEST_DIR}/share/geda-ai/applications/geda-test.desktop"
run_test "Desktop update (with file)" 0 "gEDA-AI internal Desktop Entry database updated" desktop "${TEST_DIR}/share/geda-ai/applications"
if [ ! -f "${TEST_DIR}/share/geda-ai/db/desktop.cache" ]; then
    echo "FAIL: Desktop update (with file) - Cache file ${TEST_DIR}/share/geda-ai/db/desktop.cache not created."
    cat "${LOG_FILE}"
    FAIL_COUNT=$((FAIL_COUNT + 1))
fi

# Test 4: Desktop update with no .desktop files
rm -f "${TEST_DIR}/share/geda-ai/applications/geda-test.desktop" # Ensure no files
run_test "Desktop update (no files)" 0 "No gEDA-AI .desktop files found" desktop "${TEST_DIR}/share/geda-ai/applications"

# Test 5: Invalid TYPE argument
run_test "Invalid TYPE argument" 1 "Error: Unknown TYPE 'invalidtype'" invalidtype "${TEST_DIR}/share/geda-ai/mime/packages"


# Test 6: Invalid TARGET_DIR structure (not conforming to .../share/geda-ai/...)
mkdir -p "${TEST_DIR}/bad_structure/foo/packages"
run_test "Invalid TARGET_DIR structure" 1 "Error: TARGET_DIR" mime "${TEST_DIR}/bad_structure/foo/packages"

# Test 7: Missing arguments
run_test "Missing arguments" 1 "Error: Incorrect number of arguments." 

# --- Summary ---
echo "-----------------------------------------------------"
if [ ${FAIL_COUNT} -eq 0 ]; then
  echo "All tests PASSED."
  cleanup
  exit 0
else
  echo "${FAIL_COUNT} test(s) FAILED."
  # cleanup # Optionally leave test_workdir for inspection on failure
  exit 1
fi
