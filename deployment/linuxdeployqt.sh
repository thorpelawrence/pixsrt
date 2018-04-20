#!/bin/bash
wget -c "https://github.com/probonopd/linuxdeployqt/releases/download/continuous/linuxdeployqt-continuous-x86_64.AppImage" 
chmod a+x linuxdeployqt*.AppImage
unset QTDIR; unset QT_PLUGIN_PATH ; unset LD_LIBRARY_PATH
./linuxdeployqt*.AppImage ./appdir/usr/share/applications/*.desktop -bundle-non-qt-libs
if [ -z "$desktopintegration" ] ; then ./linuxdeployqt*.AppImage ./appdir/usr/share/applications/*.desktop -appimage ; fi
if [ ! -z "$desktopintegration" ] ; then . appimage-desktopintegration.sh ; fi
find ./appdir -executable -type f -exec ldd {} \; | grep " => /usr" | cut -d " " -f 2-3 | sort | uniq
mv ./Pixsrt*.AppImage ./pixsrt-${TRAVIS_TAG}-linux-64${desktopintegration}.AppImage
curl --upload-file ./pixsrt-${TRAVIS_TAG}-linux-64${desktopintegration}.AppImage https://transfer.sh/pixsrt-${TRAVIS_TAG}-${CXX}-64${desktopintegration}.AppImage
