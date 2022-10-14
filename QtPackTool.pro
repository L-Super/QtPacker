QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++14

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    Config.cpp \
    PackTool.cpp \
    QtPath.cpp \
    ZipTool.cpp \
    main.cpp

HEADERS += \
    Config.h \
    Log.h \
    PackTool.h \
    QtPath.h \
    Version.h \
    ZipTool.h

FORMS += \
    PackTool.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

# 程序名
TARGET = QtPacker
## 版本号
#VERSION = 1.3.0
## 中文
#RC_LANG = 0x0004
## 公司名
#QMAKE_TARGET_COMPANY = LIM
## 产品名称
#QMAKE_TARGET_PRODUCT = QtPacker
## 详细描述
##QMAKE_TARGET_DESCRIPTION = Package Qt program
## 版权
#QMAKE_TARGET_COPYRIGHT = Copyright(C) 2021-2077 LIM Ltd
## 图标
#RC_ICONS = resources/yellowman.ico

RESOURCES += \
    resources.qrc

_MSC_VER{
RC_FILE += MSVC_resource.rc
message("load MSVC rc file")
}
else{
RC_FILE += MinGW_resource.rc
message("load MinGw rc file")
}

msvc {
    QMAKE_CFLAGS += /utf-8
    QMAKE_CXXFLAGS += /utf-8
}

#win32: LIBS += -L$$PWD/lib/ -lzlib -lbz2 -lzip

win32: LIBS += -L$$PWD/bin/ -lzip

INCLUDEPATH += $$PWD/include
DEPENDPATH += $$PWD/include


