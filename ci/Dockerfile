ARG base_image

FROM ${base_image}

SHELL ["bash", "-x", "-c"]

RUN export DEBIAN_FRONTEND=noninteractive && \
    apt-get update && \
    apt-get install -y build-essential cmake git gcovr patchelf wget \
            libmagic-dev libjpeg-dev libpng-dev libboost-filesystem-dev libboost-regex-dev \
            cimg-dev qtbase5-dev qtdeclarative5-dev-tools qml-module-qtquick2 qtdeclarative5-dev \
            googletest google-mock nlohmann-json3-dev autoconf libtool nano qtwebengine5-dev && \
    apt-get autoremove --purge -y && \
    apt-get clean -y

ENV CI=1

# in case AppImageLauncher is installed on the host, this little snippet will make AppImages launch normally
#RUN echo -e '#! /bin/bash\nset -exo pipefail\nexec "$@"' > /usr/bin/AppImageLauncher && \
#    chmod +x /usr/bin/AppImageLauncher

# we need to configure some Qt tools, therefore we use /tmp as temporary home
ENV HOME=/tmp

# make sure all AppImages can run in Docker
ENV APPIMAGE_EXTRACT_AND_RUN=1
