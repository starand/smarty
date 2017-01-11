#-------------------------------------------------
#
# Project created by QtCreator 2015-07-13T13:36:43
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android

OTHER_FILES += \
    android/AndroidManifest.xml \
    android/res/drawable/icon.png


TARGET = smarty
TEMPLATE = app

FW_DIR = ../../../../../../framework
JSON = ../../../../../../3rdparty/json
COMMON_DIR = ../../common
BIN_DIR = ../../../bin
ROOT_DIR = ../../

INCLUDEPATH += $${FW_DIR}/utils \
    $${FW_DIR}/common \
    $${FW_DIR}/types \
    $${FW_DIR}/utils/files \
    $${FW_DIR}/utils/threading \
    $${FW_DIR}/utils/net \
    $${FW_DIR}/utils/serialize \
    $${COMMON_DIR} \
    $${JSON}/.. \
    $${ROOT_DIR}


SOURCES += main.cpp \
    $${COMMON_DIR}/library_intf.cpp \
    $${COMMON_DIR}/client_lib.cpp \
    $${COMMON_DIR}/config_parser.cpp \
    $${FW_DIR}/utils/utils.cpp \
    $${FW_DIR}/utils/strutils.cpp \
    $${FW_DIR}/utils/files/library.cpp \
    $${FW_DIR}/utils/files/ini_parser.cpp \
    $${FW_DIR}/utils/files/fileutils.cpp \
    $${FW_DIR}/utils/files/ConfigFile.cpp \
    $${FW_DIR}/common/logger.cpp \
    $${FW_DIR}/utils/threading/threading.cpp \
    $${FW_DIR}/utils/threading/threading_linux.cpp \
    $${FW_DIR}/utils/threading/threading_windows.cpp \
    $${FW_DIR}/utils/net/xsocket.cpp \
    $${JSON}/json_reader.cpp \
    $${JSON}/json_writer.cpp \
    $${JSON}/json_value.cpp \
    DesktopList.cpp \
    ConfigDialog.cpp \
    DesktopDialog.cpp \
    MusicPlayer.cpp \
    MainWindow.cpp \
    LogViewDialog.cpp

HEADERS  += \
    DesktopList.h \
    ConfigDialog.h \
    DesktopDialog.h \
    MainWindow.h \
    MusicPlayer.h \
    LogViewDialog.h

FORMS    += \
    DesktopList.ui \
    DesktopDialog.ui \
    ConfigDialog.ui \
    MainWindow.ui \
    MusicPlayer.ui \
    LogViewDialog.ui

CONFIG += mobility
MOBILITY =

contains(ANDROID_TARGET_ARCH,armeabi) {
    ANDROID_EXTRA_LIBS = \
        $${BIN_DIR}/android/release/libclient.so
}

contains(ANDROID_TARGET_ARCH,armeabi-v7a) {
    ANDROID_EXTRA_LIBS = \
        $${BIN_DIR}/android/release/libclient.so
}

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/ -lws2_32
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/ -lws2_32

RESOURCES += \
    smarty.qrc

