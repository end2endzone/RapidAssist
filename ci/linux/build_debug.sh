# Any commands which fail will cause the shell script to exit immediately
set -e

cd ../..
export RAPIDASSIST_SOURCE_DIR=$PWD
echo "RAPIDASSIST_SOURCE_DIR set to $RAPIDASSIST_SOURCE_DIR"

# Set debug build type
export CMAKE_BUILD_TYPE=Debug

# Call build scripts one by one.
cd $RAPIDASSIST_SOURCE_DIR/ci/linux && ./install_googletest.sh;
cd $RAPIDASSIST_SOURCE_DIR/ci/linux && ./build_library.sh;
cd $RAPIDASSIST_SOURCE_DIR/ci/linux && ./build_client.sh;
cd $RAPIDASSIST_SOURCE_DIR/ci/linux && ./test_script.sh;
