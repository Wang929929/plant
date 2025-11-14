QT       += core gui multimedia multimediawidgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    audiomanager.cpp \
    card.cpp \
    fire.cpp \
    main.cpp \
    mainwindow.cpp \
    map.cpp \
    pepper.cpp \
    plant.cpp \
    potato.cpp \
    shop.cpp \
    shovel.cpp\
    sun.cpp \
    sunflower.cpp \
    zombies.cpp

HEADERS += \
    audiomanager.h \
    card.h \
    fire.h \
    mainwindow.h \
    map.h \
    peashooter.h \
    pepper.h \
    plant.h \
    potato.h \
    shop.h \
    shovel.h \
    sun.h \
    sunflower.h \
    zombies.h

FORMS += \
    mainwindow.ui

TRANSLATIONS += \
    plantt_zh_CN.ts
CONFIG += lrelease
CONFIG += embed_translations

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resources.qrc
