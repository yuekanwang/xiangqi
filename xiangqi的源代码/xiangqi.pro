QT       += core gui multimedia network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17
# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    AiGame.cpp \
    Step.cpp \
    gamescene.cpp \
    main.cpp \
    mainwindow.cpp \
    netgame.cpp \
    stone.cpp

HEADERS += \
    AiGame.h \
    Gamescene.h \
    Netgame.h \
    Step.h \
    Stone.h \
    mainwindow.h

FORMS += \
    gamescene.ui \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    music.qrc \
    res.qrc
