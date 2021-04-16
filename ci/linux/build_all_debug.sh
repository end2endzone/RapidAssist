# Any commands which fail will cause the shell script to exit immediately
set -e

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

# Set debug or release build type
export RAPIDASSIST_BUILD_TYPE=Debug
echo "RAPIDASSIST_BUILD_TYPE set to '$RAPIDASSIST_BUILD_TYPE'."

# Call all build scripts one by one.
cd "$RAPIDASSIST_SOURCE_DIR/ci/linux" && ./install_googletest.sh;
cd "$RAPIDASSIST_SOURCE_DIR/ci/linux" && ./build_library.sh;
cd "$RAPIDASSIST_SOURCE_DIR/ci/linux" && ./build_client.sh;
cd "$RAPIDASSIST_SOURCE_DIR/ci/linux" && ./test_script.sh;
