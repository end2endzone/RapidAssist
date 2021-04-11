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
echo Generating RapidAssist client example...
echo ============================================================================
cd $RAPIDASSIST_SOURCE_DIR/client
mkdir -p build
cd build
cmake -DCMAKE_BUILD_TYPE=$RAPIDASSIST_BUILD_TYPE -DCMAKE_PREFIX_PATH=$RAPIDASSIST_SOURCE_DIR/third_parties/googletest/install\;$RAPIDASSIST_SOURCE_DIR/install ..

echo ============================================================================
echo Compiling RapidAssist client example...
echo ============================================================================
cmake --build . -- -j4
echo
