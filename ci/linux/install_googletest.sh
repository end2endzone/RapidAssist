# Any commands which fail will cause the shell script to exit immediately
set -e

# Validate Travis CI environment
if [ "$RAPIDASSIST_SOURCE_DIR" = "" ]; then
  echo "Please define 'RAPIDASSIST_SOURCE_DIR' environment variable.";
  exit 1;
fi

echo ============================================================================
echo Cloning googletest into $RAPIDASSIST_SOURCE_DIR/third_parties/googletest
echo ============================================================================
mkdir -p $RAPIDASSIST_SOURCE_DIR/third_parties
cd $RAPIDASSIST_SOURCE_DIR/third_parties
git clone "https://github.com/google/googletest.git"
cd googletest
echo

echo Checking out version 1.8.0...
git -c advice.detachedHead=false checkout release-1.8.0
echo

echo ============================================================================
echo Compiling...
echo ============================================================================
mkdir -p build
cd build
cmake -DCMAKE_BUILD_TYPE=$CMAKE_BUILD_TYPE -DCMAKE_INSTALL_PREFIX=$RAPIDASSIST_SOURCE_DIR/third_parties/googletest/install -DBUILD_SHARED_LIBS=OFF -DBUILD_GMOCK=OFF -DBUILD_GTEST=ON ..
cmake --build . -- -j4
echo

echo ============================================================================
echo Installing into $RAPIDASSIST_SOURCE_DIR/third_parties/googletest/install
echo ============================================================================
make install
echo
