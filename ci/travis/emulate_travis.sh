restore_dir=$PWD
cd ../..
export TRAVIS=true
export TRAVIS_BUILD_DIR=$PWD
export RAPIDASSIST_BUILD_TYPE=Release
echo "TRAVIS_BUILD_DIR set to $TRAVIS_BUILD_DIR"
cd $restore_dir
