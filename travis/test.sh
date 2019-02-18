#!/usr/bin/env bash

set -e
set -x

if [ "$ARCH" == "" ]; then
    echo 'Error: $ARCH is not set'
    exit 1
fi

TARGET="$1"
if [ "$TARGET" == "" ]; then
    echo 'Error: $TARGET is not set'
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

wget -N https://github.com/linuxdeploy/linuxdeploy/releases/download/continuous/linuxdeploy-"$ARCH".AppImage
export LINUXDEPLOY_BIN="$PWD"/linuxdeploy-"$ARCH".AppImage
chmod +x "$LINUXDEPLOY_BIN"

pushd "$BUILD_DIR"

git clone --depth=1 https://github.com/linuxdeploy/linuxdeploy-plugin-qt-examples.git

source /opt/qt*/bin/qt*-env.sh
mkdir -p "$HOME"/.config/qtchooser
echo "${QTDIR}/bin" > "$HOME"/.config/qtchooser/qt5.10.conf
echo "${QTDIR}/lib" >> "$HOME"/.config/qtchooser/qt5.10.conf

export CMAKE_PREFIX_PATH="$QTDIR"/lib/cmake
export QT_SELECT=qt5.10


## Build projects
pushd linuxdeploy-plugin-qt-examples/QtQuickControls2Application
    # This env variable is used by the qt plugin to search the qml sources in other paths than the AppDir
    # it's mandatory to use when your qml files are embed as Qt resources into the main binary.
    export QML_SOURCES_PATHS="$PWD"/src

    mkdir build
    pushd build
        cmake .. -DCMAKE_BUILD_TYPE=RelWithDebInfo -DCMAKE_INSTALL_PREFIX=/usr || exit 1
        DESTDIR="$PWD"/AppDir make install || exit 1

        "$LINUXDEPLOY_BIN" --appdir "$PWD"/AppDir --plugin qt --output appimage || exit 1
         mv -v *AppImage "$BUILD_DIR" || exit 1
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
        cp -r /usr/lib/x86_64-linux-gnu/nss "$PWD"/AppDir/usr/lib/

        "$LINUXDEPLOY_BIN" --appdir "$PWD"/AppDir --plugin qt --output appimage || exit 1
         mv -v *AppImage "$BUILD_DIR" || exit 1
    popd
popd

pushd linuxdeploy-plugin-qt-examples/QtWidgetsApplication
    mkdir build
    pushd build
        qmake CONFIG+=release PREFIX=/usr ../QtWidgetsApplication.pro || exit 1
        INSTALL_ROOT="$PWD"/AppDir make install || exit 1

        "$LINUXDEPLOY_BIN" --appdir "$PWD"/AppDir --plugin qt --output appimage || exit 1
         mv -v *AppImage "$BUILD_DIR" || exit 1
    popd
popd
