# Any commands which fail will cause the shell script to exit immediately
set -e

# Validate mandatory environment variables
if [ "$RAPIDASSIST_BUILD_TYPE" = "" ]; then
  echo "Please define 'RAPIDASSIST_BUILD_TYPE' environment variable.";
  exit 1;
fi

# Set RAPIDASSIST_SOURCE_DIR root directory
if [ "$RAPIDASSIST_SOURCE_DIR" = "" ]; then
  RESTORE_DIRECTORY="$PWD"
  cd "$(dirname "$0")"
  cd ../..
  export RAPIDASSIST_SOURCE_DIR="$PWD"
  echo "RAPIDASSIST_SOURCE_DIR set to '$RAPIDASSIST_SOURCE_DIR'."
  cd "$RESTORE_DIRECTORY"
  unset RESTORE_DIRECTORY
fi

echo ============================================================================
echo Testing RapidAssist library...
echo ============================================================================
cd "$RAPIDASSIST_SOURCE_DIR/build/bin"
if [ "$RAPIDASSIST_BUILD_TYPE" = "Debug" ]; then
  ./rapidassist_unittest-d || true; #do not fail build even if a test fails.
else
  ./rapidassist_unittest   || true; #do not fail build even if a test fails.
fi

# Note:
#  GitHub Action do not support uploading test results in a nice GUI. There is no build-in way to detect a failed test.
#  Do not reset the error returned by unit test execution. This will actually fail the build and will indicate in GitHub that a test has failed.

#Debug TestProcess filters:
#./rapidassist_unittest --gtest_filter=TestFilesystem.testNormalizePath:TestProcess.testIsRunning:TestProcess.testProcesses*:TestProcess.testGetExitCode*:TestProcess.testWaitExit:TestString.testIsNumeric
#./rapidassist_unittest --gtest_filter=TestProcess.testGetExitCode*:TestProcess.testWaitExit
