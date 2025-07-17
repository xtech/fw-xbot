#!/bin/bash
set -euo pipefail

PRESET=${1:-Release}

mkdir -p build out

cd build
cmake .. --preset=$PRESET
cd $PRESET
make -j$(nproc)

cp -v xbot-firmware.elf ../../out/xbot-firmware.elf
cp -v xbot-firmware.bin ../../out/xbot-firmware.bin
