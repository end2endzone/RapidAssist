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

# Prepare CMAKE parameters
export CMAKE_INSTALL_PREFIX="$RAPIDASSIST_SOURCE_DIR/install"
unset CMAKE_PREFIX_PATH
export CMAKE_PREFIX_PATH="$CMAKE_PREFIX_PATH;$RAPIDASSIST_SOURCE_DIR/third_parties/googletest/install"
export CMAKE_PREFIX_PATH="$CMAKE_PREFIX_PATH;$RAPIDASSIST_SOURCE_DIR/install"

echo ============================================================================
echo Generating RapidAssist client example...
echo ============================================================================
cd "$RAPIDASSIST_SOURCE_DIR/client"
mkdir -p build
cd build
cmake -DCMAKE_BUILD_TYPE=$RAPIDASSIST_BUILD_TYPE -DCMAKE_INSTALL_PREFIX="$CMAKE_INSTALL_PREFIX" -DCMAKE_PREFIX_PATH="$CMAKE_PREFIX_PATH" ..

echo ============================================================================
echo Compiling RapidAssist client example...
echo ============================================================================
cmake --build . -- -j4
echo
