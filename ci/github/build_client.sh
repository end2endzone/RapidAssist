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
cd $GITHUB_WORKSPACE/client
mkdir -p build
cd build
cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH=$GITHUB_WORKSPACE/third_parties/googletest/install\;$GITHUB_WORKSPACE/install ..

echo ============================================================================
echo Compiling...
echo ============================================================================
cmake --build . -- -j4
echo
