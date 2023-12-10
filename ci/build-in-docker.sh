#! /bin/bash

log_message() {
    color="$1"
    shift
    if [ -t 0 ]; then tput setaf "$color"; fi
    if [ -t 0 ]; then tput bold; fi
    echo "$@"
    if [ -t 0 ]; then tput sgr0; fi
}
info() {
    log_message 2 "[info] $*"
}
warning() {
    log_message 3 "[warning] $*"
}
error() {
    log_message 1 "[error] $*"
}

if [[ "$ARCH" == "" ]]; then
    error "Usage: env ARCH=... bash $0"
    exit 2
fi

set -euo pipefail

this_dir="$(readlink -f "$(dirname "${BASH_SOURCE[0]}")")"

case "$ARCH" in
    x86_64)
        docker_arch=amd64
        ;;
    i386)
        docker_arch=i386
        ;;
    armhf)
        docker_arch=arm32v7
        ;;
    aarch64)
        docker_arch=arm64v8
        ;;
    *)
        echo "Unsupported \$ARCH: $ARCH"
        exit 3
        ;;
esac

# first, we need to build the image
# we always attempt to build it, it will only be rebuilt if Docker detects changes
# optionally, we'll pull the base image beforehand
info "Building Docker image for $ARCH (Docker arch: $docker_arch)"

build_args=()
if [[ "${UPDATE:-}" == "" ]]; then
    warning "\$UPDATE not set, base image will not be pulled!"
else
    build_args+=("--pull")
fi

docker_image=linuxdeploy-plugin-qt-build:"$ARCH"

docker build \
    --build-arg ARCH="$ARCH" \
    --build-arg docker_arch="$docker_arch" \
    "${build_args[@]}" \
    -t "$docker_image" \
    "$this_dir"/docker

docker_run_args=()

# only if there's more than 1G of free space in RAM, we can build in a RAM disk
if [[ "${GITHUB_ACTIONS:-}" != "" ]]; then
    warning "Building on GitHub actions, which does not support --tmpfs flag -> building on regular disk"
elif [[ "$(env LC_ALL=C free -m | grep "Mem:" | awk '{print $4}')" -gt 1024 ]]; then
    info "Host system has enough free memory -> building in RAM disk"
    docker_run_args+=(
        "--tmpfs"
        "/docker-ramdisk:exec,mode=777"
    )
else
    warning "Host system does not have enough free memory -> building on regular disk"
fi

if [ -t 1 ]; then
    # needed on unixoid platforms to properly terminate the docker run process with Ctrl-C
    docker_run_args+=("-t")
fi

# fix for https://stackoverflow.com/questions/51195528/rcc-error-in-resource-qrc-cannot-find-file-png
if [ "${CI:-}" != "" ]; then
    docker_args+=(
        "--security-opt"
        "seccomp:unconfined"
    )
fi

uid="${UID:-"$(id -u)"}"
info "Running build with uid $uid"

run_in_docker() {
    # run the build with the current user to
    #   a) make sure root is not required for builds
    #   b) allow the build scripts to "mv" the binaries into the /out directory
    docker run \
        --rm \
        -i \
        --init \
        -e GITHUB_RUN_NUMBER \
        -e ARCH \
        -e CI \
        --user "$uid" \
        "${docker_args[@]}" \
        -v "$(readlink -f "$this_dir"/..):/ws" \
        -w /ws \
        "$docker_image" \
        "$@"
}

run_in_docker bash ci/build.sh
run_in_docker bash ci/test.sh linuxdeploy-plugin-qt-"$ARCH".AppImage
