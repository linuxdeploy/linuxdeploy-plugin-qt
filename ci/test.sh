#!/usr/bin/env bash

set -e

if [ "$ARCH" == "" ]; then
    echo 'Error: $ARCH is not set'
    exit 1
fi

target="$1"
if [ "$target" == "" ]; then
    echo 'Usage: $0 <target.AppImage>'
    exit 1
fi

set -x

# use RAM disk if possible
if [ "$CI" == "" ] && [ -d /dev/shm ]; then
    TEMP_BASE=/dev/shm
else
    TEMP_BASE=/tmp
fi

build_dir="$(mktemp -d -p "$TEMP_BASE" linuxdeploy-plugin-qt-build-XXXXXX)"

cleanup () {
    if [ -d "$build_dir" ]; then
        rm -rf "$build_dir"
    fi
}

trap cleanup EXIT

wget -N https://github.com/linuxdeploy/linuxdeploy/releases/download/continuous/linuxdeploy-"$ARCH".AppImage

cp linuxdeploy-"$ARCH".AppImage "$build_dir"

linuxdeploy_bin="$build_dir"/linuxdeploy-"$ARCH".AppImage
chmod +x "$linuxdeploy_bin"

cp "$target" "$build_dir"

pushd "$build_dir"

git clone --depth=1 https://github.com/linuxdeploy/linuxdeploy-plugin-qt-examples.git

## Build projects
pushd linuxdeploy-plugin-qt-examples/QtQuickControls2Application
    # This env variable is used by the qt plugin to search the qml sources in other paths than the AppDir
    # it's mandatory to use when your qml files are embed as Qt resources into the main binary.
    export QML_SOURCES_PATHS="$PWD"/src

    mkdir build
    pushd build
        cmake .. -DCMAKE_BUILD_TYPE=RelWithDebInfo -DCMAKE_INSTALL_PREFIX=/usr || exit 1
        DESTDIR="$PWD"/AppDir make install || exit 1

        "$linuxdeploy_bin" --appdir "$PWD"/AppDir --plugin qt --output appimage || exit 1
        mv -v *AppImage "$build_dir" || exit 1
    popd
popd

pushd linuxdeploy-plugin-qt-examples/QtWebEngineApplication
    export QML_SOURCES_PATHS="$PWD"

    mkdir build
    pushd build
        qmake CONFIG+=release PREFIX=/usr ../QtWebEngineApplication.pro || exit 1
        INSTALL_ROOT="$PWD"/AppDir make install || exit 1

        # Include libnss related files
        mkdir -p "$PWD"/AppDir/usr/lib/
        cp -r /usr/lib/"$ARCH"-linux-gnu/nss "$PWD"/AppDir/usr/lib/

        "$linuxdeploy_bin" --appdir "$PWD"/AppDir --plugin qt --output appimage || exit 1
        mv -v *AppImage "$build_dir" || exit 1
    popd
popd

pushd linuxdeploy-plugin-qt-examples/QtWidgetsApplication
    mkdir build
    pushd build
        qmake CONFIG+=release PREFIX=/usr ../QtWidgetsApplication.pro || exit 1
        INSTALL_ROOT="$PWD"/AppDir make install || exit 1

        "$linuxdeploy_bin" --appdir "$PWD"/AppDir --plugin qt --output appimage || exit 1
        mv -v *AppImage "$build_dir" || exit 1
    popd
popd
