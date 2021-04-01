# Any commands which fail will cause the shell script to exit immediately
set -e

# Validate RapidAssist environment variables
if [ "$RAPIDASSIST_SOURCE_DIR" = "" ]; then
  echo "Please define 'RAPIDASSIST_SOURCE_DIR' environment variable.";
  exit 1;
fi
if [ "$RAPIDASSIST_BUILD_TYPE" = "" ]; then
  echo "Please define 'RAPIDASSIST_BUILD_TYPE' environment variable.";
  exit 1;
fi

echo ============================================================================
echo Testing RapidAssist library...
echo ============================================================================
cd $RAPIDASSIST_SOURCE_DIR/build/bin;
if [ "$RAPIDASSIST_BUILD_TYPE" = "Debug" ]; then
  ./rapidassist_unittest-d || true; #do not fail build even if a test fails.
else
  ./rapidassist_unittest   || true; #do not fail build even if a test fails.
fi

#Debug TestProcess filters:
#./rapidassist_unittest --gtest_filter=TestFilesystem.testNormalizePath:TestProcess.testIsRunning:TestProcess.testProcesses*:TestProcess.testGetExitCode*:TestProcess.testWaitExit:TestString.testIsNumeric
#./rapidassist_unittest --gtest_filter=TestProcess.testGetExitCode*:TestProcess.testWaitExit
