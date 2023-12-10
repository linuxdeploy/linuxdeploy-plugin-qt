#!/usr/bin/env bash

set -euo pipefail

if [ "$ARCH" == "" ]; then
    echo "Error: $ARCH is not set"
    exit 1
fi

target="$1"
if [ "$target" == "" ]; then
    echo "Usage: $0 <target.AppImage>"
    exit 1
fi

set -x

# use RAM disk if possible
if [ "${CI:-}" == "" ] && [ -d /docker-ramdisk ]; then
    TEMP_BASE=/docker-ramdisk
else
    TEMP_BASE=/tmp
fi

build_dir="$(mktemp -d -p "$TEMP_BASE" linuxdeploy-plugin-qt-test-XXXXXX)"

cleanup () {
    if [ -d "$build_dir" ]; then
        rm -rf "$build_dir"
    fi
}
trap cleanup EXIT

patch_appimage() {
    # qemu is not happy about the AppImage type 2 magic bytes, so we need to "fix" that
    dd if=/dev/zero bs=1 count=3 seek=8 conv=notrunc of="$1"
}

wget -N https://github.com/linuxdeploy/linuxdeploy/releases/download/continuous/linuxdeploy-"$ARCH".AppImage
patch_appimage linuxdeploy-"$ARCH".AppImage

cp linuxdeploy-"$ARCH".AppImage "$build_dir"

linuxdeploy_bin="$build_dir"/linuxdeploy-"$ARCH".AppImage
chmod +x "$linuxdeploy_bin"

cp "$target" "$build_dir"

pushd "$build_dir"

patch_appimage "$(basename "$target")"

# try exec
./"$(basename "$target")" || true
./"$(basename "$target")" --help || true

git clone --depth=1 https://github.com/linuxdeploy/linuxdeploy-plugin-qt-examples.git

## Build projects
pushd linuxdeploy-plugin-qt-examples/QtQuickControls2Application
    # This env variable is used by the qt plugin to search the qml sources in other paths than the AppDir
    # it's mandatory to use when your qml files are embed as Qt resources into the main binary.
    export QML_SOURCES_PATHS="$PWD"/src

    mkdir build
    pushd build
        cmake .. -DCMAKE_BUILD_TYPE=RelWithDebInfo -DCMAKE_INSTALL_PREFIX=/usr
        DESTDIR="$PWD"/AppDir make install

        "$linuxdeploy_bin" --appdir "$PWD"/AppDir --plugin qt --output appimage
        mv -v *AppImage "$build_dir"
    popd
popd

pushd linuxdeploy-plugin-qt-examples/QtWebEngineApplication
    export QML_SOURCES_PATHS="$PWD"

    mkdir build
    pushd build
        qmake CONFIG+=release PREFIX=/usr ../QtWebEngineApplication.pro
        INSTALL_ROOT="$PWD"/AppDir make install

        "$linuxdeploy_bin" --appdir "$PWD"/AppDir --plugin qt --output appimage
        mv -v ./*AppImage "$build_dir"
    popd
popd

pushd linuxdeploy-plugin-qt-examples/QtWidgetsApplication
    mkdir build
    pushd build
        qmake CONFIG+=release PREFIX=/usr ../QtWidgetsApplication.pro
        INSTALL_ROOT="$PWD"/AppDir make install

        "$linuxdeploy_bin" --appdir "$PWD"/AppDir --plugin qt --output appimage
        mv -v ./*.AppImage "$build_dir"
    popd

    mkdir build-platforms
    pushd build-platforms
        export EXTRA_PLATFORM_PLUGINS="libqoffscreen.so;libqminimal.so"
        qmake CONFIG+=release PREFIX=/usr ../QtWidgetsApplication.pro
        INSTALL_ROOT="$PWD"/AppDir make install

        env OUTPUT=platforms.AppImage "$linuxdeploy_bin" --appdir "$PWD"/AppDir --plugin qt --output appimage
        mv -v platforms.AppImage "$build_dir"
    popd
popd
