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
cd $TRAVIS_BUILD_DIR
mkdir -p build
cd build
cmake -DCMAKE_INSTALL_PREFIX=$TRAVIS_BUILD_DIR/install -DCMAKE_PREFIX_PATH=$TRAVIS_BUILD_DIR/third_parties/googletest/install -DRAPIDASSIST_BUILD_TEST=ON -DBUILD_SHARED_LIBS=OFF ..

echo ============================================================================
echo Compiling...
echo ============================================================================
cmake --build .
echo

echo ============================================================================
echo Installing into $TRAVIS_BUILD_DIR/install
echo ============================================================================
make install
echo
