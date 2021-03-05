# Any commands which fail will cause the shell script to exit immediately
set -e

# Validate Travis CI environment
if [ "$RAPIDASSIST_SOURCE_DIR" = "" ]; then
  echo "Please define 'RAPIDASSIST_SOURCE_DIR' environment variable.";
  exit 1;
fi

echo ============================================================================
echo Generating...
echo ============================================================================
cd $RAPIDASSIST_SOURCE_DIR/client
mkdir -p build
cd build
cmake -DCMAKE_BUILD_TYPE=$CMAKE_BUILD_TYPE -DCMAKE_PREFIX_PATH=$RAPIDASSIST_SOURCE_DIR/third_parties/googletest/install\;$RAPIDASSIST_SOURCE_DIR/install ..

echo ============================================================================
echo Compiling...
echo ============================================================================
cmake --build . -- -j4
echo
