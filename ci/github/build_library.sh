# Any commands which fail will cause the shell script to exit immediately
set -e

# Validate GitHub CI environment
if [ "$GITHUB_WORKSPACE" = "" ]; then
  echo "Please define 'GITHUB_WORKSPACE' environment variable.";
  exit 1;
fi

echo ============================================================================
echo Generating...
echo ============================================================================
cd $GITHUB_WORKSPACE
mkdir -p build
cd build
cmake -DCMAKE_INSTALL_PREFIX=$GITHUB_WORKSPACE/install -DCMAKE_PREFIX_PATH=$GITHUB_WORKSPACE/third_parties/googletest/install -DRAPIDASSIST_BUILD_TEST=ON -DBUILD_SHARED_LIBS=OFF ..

echo ============================================================================
echo Compiling...
echo ============================================================================
cmake --build . -- -j4
echo

echo ============================================================================
echo Installing into $GITHUB_WORKSPACE/install
echo ============================================================================
make install
echo
