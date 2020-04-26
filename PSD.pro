QT -= gui
QT += network

CONFIG += c++11 console
CONFIG -= app_bundle
INCLUDEPATH += include

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
    src/base_packet.cpp \
    src/breply_packet.cpp \
    src/data.cpp \
    src/fs_interface.cpp \
    src/interface.cpp \
    src/log.cpp \
    src/main.cpp \
    src/manager.cpp \
    src/options.cpp \
    src/packet.cpp \
    src/pcap_interface.cpp \
    src/pia_msg.cpp \
    src/pia_packet.cpp \
    src/slp_interface.cpp \
    src/udp_packet.cpp

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    include/base_packet.hpp \
    include/breply_packet.hpp \
    include/data.hpp \
    include/fs_interface.hpp \
    include/interface.hpp \
    include/log.hpp \
    include/manager.hpp \
    include/options.hpp \
    include/packet.hpp \
    include/pcap_interface.hpp \
    include/pia_msg.hpp \
    include/pia_packet.hpp \
    include/slp_interface.hpp \
    include/udp_packet.hpp

LIBS += -lssl -lcrypto -lwpcap
