# Any commands which fail will cause the shell script to exit immediately
set -e

# Validate RapidAssist environment variables
if [ "$RAPIDASSIST_SOURCE_DIR" = "" ]; then
  echo "Please define 'RAPIDASSIST_SOURCE_DIR' environment variable.";
  exit 1;
fi
if [ "$RAPIDASSIST_BUILD_TYPE" = "" ]; then
  echo "Please define 'RAPIDASSIST_BUILD_TYPE' environment variable.";
  exit 1;
fi

echo ============================================================================
echo Generating...
echo ============================================================================
cd $RAPIDASSIST_SOURCE_DIR
mkdir -p build
cd build
cmake -DCMAKE_BUILD_TYPE=$RAPIDASSIST_BUILD_TYPE -DCMAKE_INSTALL_PREFIX=$RAPIDASSIST_SOURCE_DIR/install -DCMAKE_PREFIX_PATH=$RAPIDASSIST_SOURCE_DIR/third_parties/googletest/install -DRAPIDASSIST_BUILD_TEST=ON -DBUILD_SHARED_LIBS=OFF ..

echo ============================================================================
echo Compiling...
echo ============================================================================
cmake --build . -- -j4
echo

echo ============================================================================
echo Installing into $RAPIDASSIST_SOURCE_DIR/install
echo ============================================================================
make install
echo
