#-------------------------------------------------
#
# Project created by QtCreator 2017-05-08T10:30:37
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = pixsrt
TEMPLATE = app

TAG = "0.0.0"
APPVEYOR_TAG = $$(APPVEYOR_REPO_TAG_NAME)
TRAVIS_TAG = $$(TRAVIS_TAG)

!isEmpty(APPVEYOR_TAG) {
    TAG = $$APPVEYOR_TAG
}
!isEmpty(TRAVIS_TAG) {
    TAG = $$TRAVIS_TAG
}

DEFINES += TAG=\\\"$$TAG\\\"

VERSION = $$TAG

win32:RC_ICONS += "resources/pixsrt.ico"

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += main.cpp\
        mainwindow.cpp \
    sortpixels.cpp

HEADERS  += mainwindow.h \
    sortpixels.h

FORMS    += mainwindow.ui

clang:CONFIG += c++11

RESOURCES += \
    resources.qrc

unix {
    isEmpty(PREFIX) {
        PREFIX = /usr/local
    }

    target.path = $$PREFIX/bin

    shortcutfiles.files = resources/pixsrt.desktop
    shortcutfiles.path = $$PREFIX/share/applications/
    data.files += resources/pixsrt.png
    data.path = $$PREFIX/share/pixmaps

    INSTALLS += shortcutfiles
    INSTALLS += data

    INSTALLS += target
}
