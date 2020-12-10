#! /bin/bash

set -e
set -x

if [ "$ARCH" == "" ]; then
    echo 'Error: $ARCH is not set'
    exit 1
fi

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
REPO_ROOT=$(readlink -f $(dirname $(dirname "$0")))
OLD_CWD=$(readlink -f .)

pushd "$BUILD_DIR"

if [ "$ARCH" == "i386" ]; then
    EXTRA_CMAKE_ARGS=("-DCMAKE_TOOLCHAIN_FILE=$REPO_ROOT/cmake/toolchains/i386-linux-gnu.cmake" "-DUSE_SYSTEM_CIMG=OFF")
fi

cmake "$REPO_ROOT" -DCMAKE_INSTALL_PREFIX=/usr -DCMAKE_BUILD_TYPE=RelWithDebInfo "${EXTRA_CMAKE_ARGS[@]}"

make -j$(nproc)

ctest -V

make install DESTDIR=AppDir

strip_path=$(which strip)

if [ "$ARCH" == "i386" ]; then
    # download i386 strip for i386 AppImage
    # https://github.com/linuxdeploy/linuxdeploy/issues/59
    wget http://security.ubuntu.com/ubuntu/pool/main/b/binutils/binutils-multiarch_2.24-5ubuntu14.2_i386.deb
    echo "0106f170cebf5800e863a558cad039e4f16a76d3424ae943209c3f6b0cacd511  binutils-multiarch_2.24-5ubuntu14.2_i386.deb" | sha256sum -c
    wget http://security.ubuntu.com/ubuntu/pool/main/b/binutils/binutils-multiarch-dev_2.24-5ubuntu14.2_i386.deb
    echo "ed9ca4fbbf492233228f79fae6b349a2ed2ee3e0927bdc795425fccf5fae648e  binutils-multiarch-dev_2.24-5ubuntu14.2_i386.deb" | sha256sum -c
    dpkg -x binutils-multiarch_2.24-5ubuntu14.2_i386.deb out/
    dpkg -x binutils-multiarch-dev_2.24-5ubuntu14.2_i386.deb out/
    rm binutils-multiarch*.deb
    strip_path=$(readlink -f out/usr/bin/strip)
    export LD_LIBRARY_PATH=$(readlink -f out/usr/lib)
fi

export UPD_INFO="gh-releases-zsync|linuxdeploy|linuxdeploy-plugin-qt|continuous|linuxdeploy-plugin-qt-$ARCH.AppImage"

wget https://github.com/TheAssassin/linuxdeploy/releases/download/continuous/linuxdeploy-"$ARCH".AppImage
chmod +x linuxdeploy*.AppImage
./linuxdeploy-"$ARCH".AppImage --appdir AppDir \
    -d "$REPO_ROOT"/resources/linuxdeploy-plugin-qt.desktop \
    -i "$REPO_ROOT"/resources/linuxdeploy-plugin-qt.svg \
    -e $(which patchelf) \
    -e "$strip_path" \
    --output appimage

mv linuxdeploy-plugin-qt-"$ARCH".AppImage* "$OLD_CWD"/
