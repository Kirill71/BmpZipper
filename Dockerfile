FROM ubuntu:22.04

LABEL authors="kyrylo.zharenkov"

ENV DEBIAN_FRONTEND=noninteractive

# Install required packages
RUN apt-get update && apt-get install -y \
    build-essential \
    gdb \
    cmake \
    ninja-build \
    x11vnc \
    xvfb \
    supervisor \
    xterm \
    wget \
    libgl1-mesa-dev \
    libglu1-mesa-dev \
    libasound2-dev \
    libpulse-dev \
    locales \
    libfontconfig1-dev \
    libfreetype-dev \
    libx11-dev \
    libx11-xcb-dev \
    libxext-dev \
    libxfixes-dev \
    libxi-dev \
    libxrender-dev \
    libxcb1-dev \
    libxcb-cursor0 \
    libxcb-cursor-dev \
    libxcb-glx0-dev \
    libxcb-keysyms1-dev \
    libxcb-image0-dev \
    libxcb-shm0-dev \
    libxcb-icccm4-dev \
    libxcb-sync-dev \
    libxcb-xfixes0-dev \
    libxcb-shape0-dev \
    libxcb-randr0-dev \
    libxcb-render-util0-dev \
    libxcb-util-dev \
    libxcb-xinerama0-dev \
    libxcb-xkb-dev \
    libxkbcommon-dev \
    libxkbcommon-x11-dev \
    libxrandr-dev \
    libxcursor-dev \
    libdbus-1-dev \
    at-spi2-core \
    libhunspell-dev \
    vim \
    systemd-coredump \
    && apt-get clean \
    && rm -rf /var/lib/apt/lists/*

# Configure locales
RUN locale-gen C.UTF-8 || true \
    && update-locale LANG=C.UTF-8

# Set environment variables
ENV LANG=C.UTF-8
ENV LC_ALL=C.UTF-8
ENV DISPLAY=:1
ENV QT_DEBUG_PLUGINS=1
ENV QT_QPA_PLATFORM=xcb
ENV QT_PATH=/opt/qt6.7.2/bin
ENV SERVER_PORT=5900

# Configure core dumps
RUN echo "kernel.core_pattern=|/lib/systemd/systemd-coredump %P %u %g %s %t %c %h %e" > /etc/sysctl.d/50-coredump.conf \
    && sysctl -p /etc/sysctl.d/50-coredump.conf

# Install Qt 6.7.2
WORKDIR /opt
RUN wget https://download.qt.io/official_releases/qt/6.7/6.7.2/single/qt-everywhere-src-6.7.2.tar.xz \
    && tar -xf qt-everywhere-src-6.7.2.tar.xz \
    && rm qt-everywhere-src-6.7.2.tar.xz \
    && cd qt-everywhere-src-6.7.2 \
    && ./configure -prefix /opt/qt6.7.2 -release -opengl desktop \
    && cmake --build . --parallel \
    && cmake --install .

# Add Qt to PATH
ENV PATH="${QT_PATH}:${PATH}"

# Create application directory and copy source code
WORKDIR /app
COPY . /app

# Build your application using Make
RUN make build

# Copy the VNC start script and set execute permissions
COPY scripts/start-vnc.sh /opt/scripts/start-vnc.sh
RUN chmod +x /opt/scripts/start-vnc.sh

# Expose the VNC port
EXPOSE $SERVER_PORT

# Set the command to run the VNC server and application
CMD ["/opt/scripts/start-vnc.sh"]