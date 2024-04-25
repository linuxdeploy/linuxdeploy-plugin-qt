#! /bin/bash

set -e
set -x

if [ "$ARCH" == "" ]; then
    echo 'Error: $ARCH is not set'
    exit 1
fi

# use RAM disk if possible
if [ "$CI" == "" ] && [ -d /docker-ramdisk ]; then
    TEMP_BASE=/docker-ramdisk
else
    TEMP_BASE=/tmp
fi

BUILD_DIR="$(mktemp -d -p "$TEMP_BASE" linuxdeploy-plugin-qt-build-XXXXXX)"

cleanup () {
    if [ -d "$BUILD_DIR" ]; then
        rm -rf "$BUILD_DIR"
    fi
}

trap cleanup EXIT

# store repo root as variable
REPO_ROOT="$(readlink -f "$(dirname "$(dirname "$0")")")"
OLD_CWD="$(readlink -f .)"

pushd "$BUILD_DIR"

cmake "$REPO_ROOT" -DCMAKE_INSTALL_PREFIX=/usr -DCMAKE_BUILD_TYPE=RelWithDebInfo -DBUILD_TESTING=ON -DSTATIC_BUILD=ON

make -j"$(nproc)"

ctest -V --no-tests=error

make install DESTDIR=AppDir

patchelf_path="$(which patchelf)"
strip_path="$(which strip)"

export UPD_INFO="gh-releases-zsync|linuxdeploy|linuxdeploy-plugin-qt|continuous|linuxdeploy-plugin-qt-$ARCH.AppImage"
export OUTPUT="linuxdeploy-plugin-qt-$ARCH.AppImage"

# special set of builds using a different experimental runtime, used for testing purposes
if [[ "${USE_STATIC_RUNTIME:-}" != "" ]]; then
    custom_runtime_url="https://github.com/AppImage/type2-runtime/releases/download/continuous/runtime-$ARCH"
    wget "$custom_runtime_url"
    runtime_filename="$(echo "$custom_runtime_url" | rev | cut -d/ -f1 | rev)"
    LDAI_RUNTIME_FILE="$(readlink -f "$runtime_filename")"
    export LDAI_RUNTIME_FILE
    export OUTPUT="linuxdeploy-plugin-qt-static-$ARCH.AppImage"
fi

wget "https://github.com/TheAssassin/linuxdeploy/releases/download/continuous/linuxdeploy-$ARCH.AppImage"
# qemu is not happy about the AppImage type 2 magic bytes, so we need to "fix" that
dd if=/dev/zero bs=1 count=3 seek=8 conv=notrunc of=linuxdeploy-"$ARCH".AppImage
chmod +x linuxdeploy*.AppImage

./linuxdeploy-"$ARCH".AppImage --appdir AppDir \
    -d "$REPO_ROOT"/resources/linuxdeploy-plugin-qt.desktop \
    -i "$REPO_ROOT"/resources/linuxdeploy-plugin-qt.svg \
    -e "$patchelf_path" \
    -e "$strip_path" \
    --output appimage

mv "$OUTPUT"* "$OLD_CWD"/
