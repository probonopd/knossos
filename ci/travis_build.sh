#!/bin/bash
set -euxo pipefail

file /usr/lib/libQt5Core.so.5.11.2
/usr/lib/libQt5Core.so.5.11.2

time pacman -Syu --noconfirm

file /usr/lib/libQt5Core.so.5.12.0
/usr/lib/libQt5Core.so.5.12.0

cd ~

# run cmake and ninja
mkdir knossos-build
cd knossos-build
cmake -G Ninja -DCMAKE_BUILD_TYPE=RELEASE -DCMAKE_PREFIX_PATH="/root/PythonQt-install/lib/cmake/" ../knossos
time ninja

# create AppImage
time ../knossos/installer/create_appimage.sh

# Deploy
BRANCH_PREFIX=""
if [[ $TRAVIS_BRANCH != "master" ]]; then
	BRANCH_PREFIX=${TRAVIS_BRANCH}-
fi
cp deploy/*.AppImage ../knossos/linux.${BRANCH_PREFIX}KNOSSOS.nightly.AppImage
