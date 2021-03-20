# Any commands which fail will cause the shell script to exit immediately
set -e

# Set RAPIDASSIST_SOURCE_DIR root directory
RESTORE_DIRECTORY=$PWD
cd "$(dirname "$0")"
cd ../..
export RAPIDASSIST_SOURCE_DIR=$PWD
echo "RAPIDASSIST_SOURCE_DIR set to $RAPIDASSIST_SOURCE_DIR."
cd $RESTORE_DIRECTORY
unset RESTORE_DIRECTORY

# Set debug or release build type
export RAPIDASSIST_BUILD_TYPE=Debug
#export RAPIDASSIST_BUILD_TYPE=Release
echo "RAPIDASSIST_BUILD_TYPE set to $RAPIDASSIST_BUILD_TYPE configuration."