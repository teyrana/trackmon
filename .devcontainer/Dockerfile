FROM fedora:36


# ====== define user ======
ARG USER_NAME=vscode
ARG USER_UID=1001
ARG USER_GID=$USER_UID
RUN useradd $USER_NAME
RUN echo "${USER_NAME}:${USER_NAME}" | chpasswd
RUN if [ "$USER_GID" != "1000" ] || [ "$USER_UID" != "1000" ]; then \
    groupmod --gid $USER_GID $USER_NAME \
    && usermod --uid $USER_UID --gid $USER_GID --group=wheel $USER_NAME \
    && chown -R $USER_UID:$USER_GID /home/$USER_NAME; \
fi


# ====== Basic Development Utilities ======
RUN dnf makecache \
    && dnf install -y \
        cmake \
        curl \
        cppcheck \
        gcc-c++ \
        gdb \
        git \
        proj-devel \
        ninja-build \
        tar \
        valgrind \
        which \
        zip


# Install vcpkg itself: https://github.com/microsoft/vcpkg/blob/master/README.md#quick-start-unix

# Setup ENV vars for vcpkg
ENV VCPKG_ROOT=/opt/vcpkg \
    VCPKG_DOWNLOADS=/opt/vcpkg/downloads \
    PATH="${PATH}:${VCPKG_ROOT}"
RUN git clone https://github.com/microsoft/vcpkg /opt/vcpkg
RUN mkdir -p ${VCPKG_DOWNLOADS} \
    && ${VCPKG_ROOT}/bootstrap-vcpkg.sh


# # not sure if this is still needed?
# COPY library-scripts/meta.env /usr/local/etc/vscode-dev-containers


# ====== Dependencies: Common Libraries ======

# Install needed packages. Use a separate RUN statement to add your own dependencies.
RUN dnf makecache \
    && dnf install -y \
        libpcap-devel \
        proj-devel \
        ncurses-devel

# ====== Dependency: libAIS ======
RUN git clone https://github.com/schwehr/libais.git /opt/libais
RUN cd /opt/libais \
    && cmake . \
    && make install 


# ====== perform final configuration ======
ENV TERM=xterm 
USER vscode
# technically redundant, since vscode will enforce this location, anyway
WORKDIR /workspaces
CMD bash
