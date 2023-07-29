QT += widgets serialport

TARGET = blockingmaster
TEMPLATE = app

HEADERS += \
    dialog.h \
    masterthread.h \
    AES.h \
    AESdef.h \
    user_defines.h

SOURCES += \
    main.cpp \
    dialog.cpp \
    masterthread.cpp \
    AES.cpp

DISTFILES +=
