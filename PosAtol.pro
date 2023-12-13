QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    atol.cpp \
    logger.cpp \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    atol.h \
    atol/include/libfptr10.h \
    logger.h \
    mainwindow.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target


linux:!android:!macx: {
    QMAKE_LFLAGS += -Wl,-rpath,"$$PWD/libs/linux_x64/"

    LIBS += \
        -L$$PWD/libs/linux_x64/ -lfptr10

    INCLUDEPATH += \
        $$PWD/libs/linux_x64

    DEPENDPATH += \
        $$PWD/libs//linux_x64
}

win32: {
    LIBS += -lWS2_32 \
        -luser32 \
        -L$$PWD/libs/msvc2015_x86 -lfptr10

    INCLUDEPATH += \
        $$PWD/libs/msvc2015_x86

    DEPENDPATH += \
        $$PWD/libs/msvc2015_x86
}
