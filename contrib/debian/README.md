
Debian
====================
This directory contains files used to package xc3dxd/xc3dx-qt
for Debian-based Linux systems. If you compile xc3dxd/xc3dx-qt yourself, there are some useful files here.

## xc3dx: URI support ##


xc3dx-qt.desktop  (Gnome / Open Desktop)
To install:

	sudo desktop-file-install xc3dx-qt.desktop
	sudo update-desktop-database

If you build yourself, you will either need to modify the paths in
the .desktop file or copy or symlink your xc3dxqt binary to `/usr/bin`
and the `../../share/pixmaps/xc3dx128.png` to `/usr/share/pixmaps`

xc3dx-qt.protocol (KDE)

