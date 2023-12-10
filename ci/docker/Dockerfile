# generic Dockerfile for all architectures
# used to "cache" prebuilt binaries of tools we use internally and installed dependencies
# needs to be re-run in CI every time as we cannot store auto-built Docker images due to GitHub's strict quota
# it will save a lot of time in local development environments, though

ARG docker_arch

# we'll just use Debian as a base image for now, mainly because it produces less headache than Ubuntu with arm
# a big pro is that they ship an up to date CMake in their stable distribution
# also, they still provide IA-32 builds for some reason...
# some people in the AppImage community do not (want to) realize i386 is dead for good
# we are going to drop i686 in the future!
FROM ${docker_arch}/debian:stable

# variables that need to be availabe during build and runtime must(!) be repeated after FROM
ARG ARCH


SHELL ["bash", "-x", "-c"]

RUN export DEBIAN_FRONTEND=noninteractive && \
    apt-get update && \
    apt-get install -y build-essential cmake git gcovr patchelf wget \
            libmagic-dev libjpeg-dev libpng-dev libboost-filesystem-dev libboost-regex-dev \
            cimg-dev qtbase5-dev qtdeclarative5-dev-tools qml-module-qtquick2 qtdeclarative5-dev \
            googletest google-mock nlohmann-json3-dev autoconf libtool nano qtwebengine5-dev gdb && \
    apt-get autoremove --purge -y && \
    apt-get clean -y

# install into separate destdir to avoid polluting the $PATH with tools like ld that will break things
ENV TOOLS_DIR=/tools

COPY install-static-binutils.sh /
RUN bash /install-static-binutils.sh

COPY install-static-patchelf.sh /
RUN bash /install-static-patchelf.sh

# make patchelf and strip available in $PATH
# they are static binaries, so we can just copy them
RUN cp "$TOOLS_DIR"/usr/bin/patchelf /usr/local/bin && \
    cp "$TOOLS_DIR"/usr/bin/strip /usr/local/bin

ENV CI=1

# in case AppImageLauncher is installed on the host, this little snippet will make AppImages launch normally
#RUN echo -e '#! /bin/bash\nset -exo pipefail\nexec "$@"' > /usr/bin/AppImageLauncher && \
#    chmod +x /usr/bin/AppImageLauncher

# we need to configure some Qt tools, therefore we use /tmp as temporary home
ENV HOME=/tmp

# make sure all AppImages can run in Docker
ENV APPIMAGE_EXTRACT_AND_RUN=1

# seems necessary to make test.sh run on qemu/arm
RUN apt-get update && \
    apt-get install -y qemu-user-static && \
    apt-get clean -y
