#!/bin/bash
cp ./resources/pixsrt.desktop ./appdir
cp ./resources/pixsrt.png ./appdir
mkdir -p ./appdir/usr/share/icons/default/256x256/apps/
cp ./resources/Pixsrt256.png ./appdir/usr/share/icons/default/256x256/apps/Pixsrt.png
wget -c "https://github.com/probonopd/AppImageKit/raw/master/desktopintegration" -O ./appdir/usr/bin/pixsrt.wrapper
chmod a+x ./appdir/usr/bin/pixsrt.wrapper
( cd ./appdir/ ; rm AppRun ; ln -s ./usr/bin/pixsrt.wrapper AppRun )
wget -c "https://github.com/AppImage/AppImageKit/releases/download/continuous/appimagetool-x86_64.AppImage"
chmod a+x appimagetool-x86_64.AppImage
./appimagetool-x86_64.AppImage ./appdir/ 
