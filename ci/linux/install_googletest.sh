# Any commands which fail will cause the shell script to exit immediately
set -e

# Validate mandatory environment variables
if [ "$RAPIDASSIST_BUILD_TYPE" = "" ]; then
  echo "Please define 'RAPIDASSIST_BUILD_TYPE' environment variable.";
  exit 1;
fi

# Find the project root directory
current_directory=$PWD
this_filename=`basename "$0"`
this_directory=`dirname "$0"`
cd $this_directory/../..
RAPIDASSIST_SOURCE_DIR=$PWD
cd $current_directory

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
echo Compiling googletest...
echo ============================================================================
mkdir -p build
cd build
cmake -DCMAKE_BUILD_TYPE=$RAPIDASSIST_BUILD_TYPE -DCMAKE_INSTALL_PREFIX=$RAPIDASSIST_SOURCE_DIR/third_parties/googletest/install -DBUILD_SHARED_LIBS=OFF -DBUILD_GMOCK=OFF -DBUILD_GTEST=ON ..
cmake --build . -- -j4
echo

echo ============================================================================
echo Installing googletest into $RAPIDASSIST_SOURCE_DIR/third_parties/googletest/install
echo ============================================================================
make install
echo
