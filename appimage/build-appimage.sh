#!/usr/bin/env bash

version=0.1.0

# get the latest opengl loader files
glad --generator c --out-path ext/glad

# build the app
rm -rf build
cmake --config Release -S . -B build -DCMAKE_INSTALL_PREFIX=/usr
cmake --build build -j $(nproc) --target install DESTDIR=AppDir

# build the appimage
cd build
wget -nc "https://github.com/linuxdeploy/linuxdeploy/releases/download/continuous/linuxdeploy-x86_64.AppImage"
chmod +x linuxdeploy-x86_64.AppImage
cat > igl.desktop <<EOF
[Desktop Entry]
Type=Application
Name=igl
GenericName=Image OpenGL
Comment=igl -- an OpenGL based app for quick image visualization.
Icon=igl
Exec=igl
Terminal=false
Categories=Utility;
EOF
./linuxdeploy-x86_64.AppImage \
    --appdir AppDir \
    -i ../igl.png \
    -d igl.desktop \
    --output appimage
