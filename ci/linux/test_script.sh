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
  ./rapidassist_unittest-d
else
  ./rapidassist_unittest
fi
