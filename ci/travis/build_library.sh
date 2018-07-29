# Any commands which fail will cause the shell script to exit immediately
set -e

# Validate Travis CI environment
if [ "$TRAVIS_BUILD_DIR" = "" ]; then
  echo "Please define 'TRAVIS_BUILD_DIR' environment variable.";
  exit 1;
fi

export GTEST_ROOT=$TRAVIS_BUILD_DIR/third_parties/googletest/install
export INSTALL_LOCATION=$TRAVIS_BUILD_DIR/install

echo ============================================================================
echo Generating...
echo ============================================================================
cd $TRAVIS_BUILD_DIR
mkdir build >/dev/null 2>/dev/null
cd build
cmake -DCMAKE_INSTALL_PREFIX=$INSTALL_LOCATION -DRAPIDASSIST_BUILD_TEST=ON -DBUILD_SHARED_LIBS=OFF ../library

echo ============================================================================
echo Compiling...
echo ============================================================================
cmake --build .
echo

echo ============================================================================
echo Installing into $INSTALL_LOCATION
echo ============================================================================
make install
echo

# Delete all temporary environment variable created
unset GTEST_ROOT
unset INSTALL_LOCATION
