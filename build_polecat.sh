set -e

TEMP_DIR="$(mktemp -d)"
function cleanup {
  rm -rf $TEMP_DIR
}
trap cleanup EXIT

cd $TEMP_DIR

echo ======== CLONING ========

git clone https://github.com/Jan200101/polecat repo
cd repo

echo ======== BUILDING ========

mkdir build
cd build
cmake .. -DBUILD_MOCK=ON -DENABLE_LUTRIS=ON
make

echo ======== RUNNING TESTS ========

export XDG_DATA_HOME="$TEMP_DIR/data"

./polecat_mock wine list
./polecat_mock wine download mock
./polecat_mock wine run mock test
./polecat_mock wine remove mock
