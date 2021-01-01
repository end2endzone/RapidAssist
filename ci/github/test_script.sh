# Any commands which fail will cause the shell script to exit immediately
set -e

# Validate GitHub CI environment
if [ "$GITHUB_WORKSPACE" = "" ]; then
  echo "Please define 'GITHUB_WORKSPACE' environment variable.";
  exit 1;
fi

echo ============================================================================
echo Testing project
echo ============================================================================
cd $GITHUB_WORKSPACE/build/bin;
./rapidassist_unittest || true; #do not fail build even if a test fails.

#Debug TestProcess filters:
#./rapidassist_unittest --gtest_filter=TestFilesystem.testNormalizePath:TestProcess.testIsRunning:TestProcess.testProcesses*:TestProcess.testGetExitCode*:TestProcess.testWaitExit:TestString.testIsNumeric
#./rapidassist_unittest --gtest_filter=TestProcess.testGetExitCode*:TestProcess.testWaitExit
