QT -= gui
QT += core
QT += serialport
QT += testlib

CONFIG += c++17 console
CONFIG -= app_bundle

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        main.cpp \
        radar_v2.cpp \
        time_measure.cpp \
        tlv_dat.cpp \
        tlv_dat_v2.cpp

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    cfgFiles.h \
    radar_v2.h \
    time_measure.h \
    tlv_dat.h \
    tlv_dat_v2.h


