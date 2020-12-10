#! /bin/bash

set -e
set -x

ubuntu_release="$(cat /etc/*release | grep -E '^DISTRIB_CODENAME=' | cut -d= -f2-)"

sudo add-apt-repository -y ppa:beineri/opt-qt-5.14.2-"$ubuntu_release"
sudo apt-get update

packages=(
    libmagic-dev libjpeg-dev libpng-dev cimg-dev mesa-common-dev

    # dependencies of patchelf
    automake gcc g++

    # up to date Qt with required plugins
    qt514-meta-minimal qt514declarative qt514webengine
)

if [[ "$ubuntu_release" == "xenial" ]]; then
    packages+=(libgl1-mesa-dev)
else
    packages+=(libglvnd-dev)
fi

sudo apt-get install -y "${packages[@]}"
