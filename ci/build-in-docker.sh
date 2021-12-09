#! /bin/bash

set -exo pipefail

case "$ARCH" in
    x86_64)
        base_image=debian:latest
        ;;
    i386)
        base_image=i386/debian:latest
        ;;
    *)
        echo "Usage: env ARCH=[x86_64|i386] $0"
        exit 2
esac

here="$(readlink -f "$(dirname "$0")")"
cd "$here"

docker_image=linuxdeploy-plugin-qt-build:"$ARCH"

docker build -t "$docker_image" --build-arg base_image="$base_image" .

if isatty &>/dev/null; then
    extra_args=("-t")
fi

run_in_docker() {
    docker run -e ARCH --rm -i "${extra_args[@]}" --init -w /ws -v "$(readlink -f "$here"/..)":/ws --user "$(id -u)" "$docker_image" "$@"
}

run_in_docker bash ci/build.sh
run_in_docker bash ci/test.sh linuxdeploy-plugin-qt-"$ARCH".AppImage
