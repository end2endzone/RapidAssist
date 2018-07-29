# Any commands which fail will cause the shell script to exit immediately
set -e

# Validate Travis CI environment
if [ "$TRAVIS_BUILD_DIR" = "" ]; then
  echo "Please define 'TRAVIS_BUILD_DIR' environment variable.";
  exit 1;
fi

echo ============================================================================
echo Cloning googletest into $TRAVIS_BUILD_DIR/third_parties/googletest
echo ============================================================================
mkdir $TRAVIS_BUILD_DIR/third_parties >/dev/null 2>/dev/null
cd $TRAVIS_BUILD_DIR/third_parties
git clone "https://github.com/google/googletest.git"
cd googletest
echo

echo Checking out version 1.8.0...
git checkout release-1.8.0
echo

echo ============================================================================
echo Compiling...
echo ============================================================================
mkdir build >/dev/null 2>/dev/null
cd build
export GTEST_ROOT=$TRAVIS_BUILD_DIR/third_parties/googletest/install
cmake -DCMAKE_INSTALL_PREFIX=$GTEST_ROOT -DCMAKE_BUILD_TYPE=Release -DBUILD_SHARED_LIBS=OFF ..
cmake --build .
echo

echo ============================================================================
echo Installing into $GTEST_ROOT
echo ============================================================================
make install
echo
