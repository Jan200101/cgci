#!/bin/bash
set -e

TEMP_DIR="$(mktemp -d)"
function cleanup {
  rm -rf $TEMP_DIR
}
trap cleanup EXIT

cd $TEMP_DIR

git clone https://github.com/lite-xl/lite-xl repo
cd repo

meson build -Dwrap_mode=forcefallback
ninja -C build
