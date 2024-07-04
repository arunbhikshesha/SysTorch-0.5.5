#!/usr/bin/env bash

# Test if in project dir
[[ -r cross.cmake ]] || { 
    echo "$0: cross.cmake not found, please call the script from the project root dir!" >&2; 
    exit 2;
}

# Test if ARMGCC_DIR of Arm GNU Toolchain is set
if [[ -z "${ARMGCC_DIR}" ]]; then {
  cat >&2 << EOF
  ARMGCC_DIR not set, please install Arm GNU Toolchain and set environment like e.g.:
    export ARMGCC_DIR="/home/dev/gcc-arm-none-eabi-10-2020-q4-major"
    export PATH="/home/dev/gcc-arm-none-eabi-10-2020-q4-major/bin:\${PATH}"
EOF
  exit 1
}
fi
echo "Using ARMGCC_DIR: $ARMGCC_DIR"

# 
path_to_gcc=$(which arm-none-eabi-gcc)
[[ -x "$path_to_gcc" ]] || { 
    echo "arm-none-eabi-gcc not found, please check correct Arm GNU Toolchain installation" >&2; 
    exit 2;
}
echo "Using Compiler Path: $path_to_gcc"

cmake -S . -G Ninja -B build -DCMAKE_TOOLCHAIN_FILE=cross.cmake -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release --target build_and_encrypt
