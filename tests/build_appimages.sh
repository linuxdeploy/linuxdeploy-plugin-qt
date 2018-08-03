#! /bin/bash

if [ "$1" == "" ]; then
    echo "Usage: $0 <path to linuxdeploy-plugin-qt>"
    exit 1
fi

set -e
set -x

linuxdeploy_plugin_qt_path=$(readlink -f "$1")
echo "Using linuxdeploy-plugin-qt: $linuxdeploy_plugin_qt_path"

old_cwd=$(pwd)
src_dir=$(readlink -f $(dirname "$0"))/"QtQuickControls2Application"

ARCH="x86_64"

build_dir=$(mktemp -d /tmp/linuxdeploy-plugin-qt-test-build-XXXXXX)

pushd "$build_dir"

mkdir build
cd build

cmake "$src_dir" -DCMAKE_BUILD_TYPE=RelWithDebInfo -DCMAKE_INSTALL_PREFIX=/usr
DESTDIR=AppDir make install

wget -N https://github.com/linuxdeploy/linuxdeploy/releases/download/continuous/linuxdeploy-"$ARCH".AppImage
chmod +x linuxdeploy-"$ARCH".AppImage

./linuxdeploy-"$ARCH".AppImage --app-name "$src_dir" --appdir AppDir --init-appdir --output appimage
export LINUXDEPLOY_QT_PLUGIN_QML_SOURCES_PATHS="$src_dir"
"$linuxdeploy_plugin_qt_path" --appdir AppDir
./linuxdeploy-"$ARCH".AppImage --app-name "$src_dir" --appdir AppDir --output appimage

cp *.AppImage "$old_cwd"

popd
