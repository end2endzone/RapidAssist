# Any commands which fail will cause the shell script to exit immediately
set -e

# Call all build scripts one by one.
cd $RAPIDASSIST_SOURCE_DIR/ci/linux && ./install_googletest.sh;
cd $RAPIDASSIST_SOURCE_DIR/ci/linux && ./build_library.sh;
cd $RAPIDASSIST_SOURCE_DIR/ci/linux && ./build_client.sh;
cd $RAPIDASSIST_SOURCE_DIR/ci/linux && ./test_script.sh;
