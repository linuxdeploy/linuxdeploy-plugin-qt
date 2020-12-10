#! /bin/bash

if [[ "$CI" == "" ]]; then
    echo "Error: this script is supposed to be run in a (disposable) CI environment"
    exit 2
fi

if [[ "$ARCH" == "" ]]; then
    echo "Usage: env ARCH=... bash $0"
    exit 3
fi

build_dir="$(mktemp -d /tmp/patchelf-build-XXXXX)"

cleanup() {
    rm -rf "$build_dir"
}
trap cleanup EXIT

pushd "$build_dir"

git clone https://github.com/NixOS/patchelf.git -b 0.8 .

./bootstrap.sh

EXTRA_CONFIGURE_ARGS=()

if [ "$ARCH" == "i386" ]; then
    export EXTRA_CONFIGURE_ARGS=("--build=i686-pc-linux-gnu" "CFLAGS=-m32" "CXXFLAGS=-m32" "LDFLAGS=-m32")
fi

./configure --prefix=/usr "${EXTRA_CONFIGURE_ARGS[@]}"

make -j$(nproc)

sudo make install
