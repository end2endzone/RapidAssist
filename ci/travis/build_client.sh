# Any commands which fail will cause the shell script to exit immediately
set -e

# Validate Travis CI environment
if [ "$TRAVIS_BUILD_DIR" = "" ]; then
  echo "Please define 'TRAVIS_BUILD_DIR' environment variable.";
  exit 1;
fi

echo ============================================================================
echo Generating...
echo ============================================================================
cd $TRAVIS_BUILD_DIR/client
mkdir -p build
cd build
cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH=$TRAVIS_BUILD_DIR/third_parties/googletest/install\;$TRAVIS_BUILD_DIR/install ..

echo ============================================================================
echo Compiling...
echo ============================================================================
cmake --build . --parallel
echo
