#-------------------------------------------------
#
# Project created by QtCreator 2014-08-16T23:05:07
#
#-------------------------------------------------

QT       -= core gui

TARGET = client
TEMPLATE = lib

DEFINES += CLIENT_LIBRARY

FW_DIR = ../../../../../../framework
COMMON_DIR = ../../common
ROOT_DIR = ../..
BIN_DIR = ../../../bin

ANDROID_DEPLOYMENT_SETTINGS_FILE = android-libsmarty.so-deployment-settings.json

Release:DESTDIR = $${BIN_DIR}
Release:OBJECTS_DIR = $${BIN_DIR}/.obj
Release:MOC_DIR = $${BIN_DIR}/.moc
Release:RCC_DIR = $${BIN_DIR}/.rcc
Release:UI_DIR = $${BIN_DIR}/.ui

Debug:DESTDIR = $${BIN_DIR}
Debug:OBJECTS_DIR = $${BIN_DIR}/.obj
Debug:MOC_DIR = $${BIN_DIR}/.moc
Debug:RCC_DIR = $${BIN_DIR}/.rcc
Debug:UI_DIR = $${BIN_DIR}/.ui

INCLUDEPATH += \
    $${FW_DIR}/types \
    $${FW_DIR}/utils \
    $${FW_DIR}/common \
    $${FW_DIR}/utils/files \
    $${FW_DIR}/utils/net \
    $${FW_DIR}/utils/threading \
    $${FW_DIR}/utils/serialize \
    $${COMMON_DIR} \
    $${ROOT_DIR}

SOURCES += \
    ../../libclient/command_sender.cpp \
    ../../libclient/libclient_interface.cpp \
    ../../libclient/notification_listener.cpp \
    ../../libclient/smarty_connector.cpp \
    ../../../../../../framework/utils/strutils.cpp \
    ../../../../../../framework/utils/files/fileutils.cpp \
    ../../../../../../framework/utils/files/ini_parser.cpp \
    ../../../../../../framework/utils/utils.cpp \
    ../../../../../../framework/utils/net/xsocket.cpp \
    ../../../../../../framework/utils/threading/threading.cpp \
    ../../../../../../framework/utils/threading/threading_linux.cpp \
    ../../../../../../framework/utils/threading/threading_windows.cpp \
    ../../../../../../framework/common/logger.cpp

HEADERS += \
    ../../libclient/command_sender.h \
    ../../libclient/notification_listener.h \
    ../../libclient/smarty_connector.h \
    ../../common/client_protocol.h \
    ../../../../../../framework/utils/strutils.h \
    ../../../../../../framework/utils/utils.h \
    ../../../../../../framework/utils/net/xsocket.h \
    ../../../../../../framework/utils/threading/threading.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}

OTHER_FILES +=

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/ -lws2_32
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/ -lws2_32


