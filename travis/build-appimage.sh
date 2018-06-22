#! /bin/bash

set -e
set -x

# use RAM disk if possible
if [ "$CI" == "" ] && [ -d /dev/shm ]; then
    TEMP_BASE=/dev/shm
else
    TEMP_BASE=/tmp
fi

BUILD_DIR=$(mktemp -d -p "$TEMP_BASE" linuxdeploy-plugin-qt-build-XXXXXX)

cleanup () {
    if [ -d "$BUILD_DIR" ]; then
        rm -rf "$BUILD_DIR"
    fi
}

trap cleanup EXIT

# store repo root as variable
REPO_ROOT=$(readlink -f $(dirname $(dirname $0)))
OLD_CWD=$(readlink -f .)

pushd "$BUILD_DIR"

cmake "$REPO_ROOT" -DCMAKE_INSTALL_PREFIX=/usr -DCMAKE_BUILD_TYPE=RelWithDebInfo

make -j$(nproc)

make install DESTDIR=AppDir

wget https://github.com/TheAssassin/linuxdeploy/releases/download/continuous/linuxdeploy-x86_64.AppImage
chmod +x linuxdeploy*.AppImage
./linuxdeploy-x86_64.AppImage -n linuxdeploy-plugin-qt --appdir AppDir --init-appdir \
    -d "$REPO_ROOT"/resources/linuxdeploy-plugin-qt.desktop \
    -i "$REPO_ROOT"/resources/linuxdeploy-plugin-qt.svg \
    -e $(which patchelf) \
    --output appimage

mv linuxdeploy-plugin-qt*.AppImage "$OLD_CWD"/
