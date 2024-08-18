#!/bin/bash
# Set VNC password
mkdir -p /root/.vnc
echo "$PASSWORD" | vncpasswd -f > /root/.vnc/passwd
chmod 600 /root/.vnc/passwd

# Start the X virtual framebuffer and VNC server
Xvfb :1 -screen 0 1280x800x16 &
x11vnc -display :1 -forever -shared -rfbport "$SERVER_PORT" &

# Start the application
cd /app || exit
make run

# Keep the container running
tail -f /dev/null