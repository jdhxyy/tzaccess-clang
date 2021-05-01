TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
        main.c \
    ../../tzaccess.c \
    ../../tzaccessapply.c \
    ../../tzaccessconfig.c \
    ../../tzaccessconn.c \
    ../../tzaccessparsecmp.c \
    ../../tzaccessstandardlayer.c \
    ../../lib/async-clang/async.c \
    ../../lib/crc16-clang/crc16.c \
    ../../lib/knock-clang/knock.c \
    ../../lib/lagan-clang/lagan.c \
    ../../lib/tzbox/tzbox.c \
    ../../lib/tzfifo/tzfifo.c \
    ../../lib/tzlist/tzlist.c \
    ../../lib/tzmalloc/bget.c \
    ../../lib/tzmalloc/tzmalloc.c \
    ../../lib/tztime/tztime.c \
    ../../lib/utz-clang/utzcommon.c \
    ../../lib/utz-clang/utzehp.c \
    ../../lib/utz-clang/utzflp.c \
    ../../lib/utz-clang/utznlv1.c

LIBS += -lpthread libwsock32 libws2_32

INCLUDEPATH += ../../ \
    ../../lib/utz-clang \
    ../../lib/tztime \
    ../../lib/tzmalloc \
    ../../lib/tzlist \
    ../../lib/tzfifo \
    ../../lib/lagan-clang \
    ../../lib/crc16-clang \
    ../../lib/pt \
    ../../lib/tzbox \
    ../../lib/knock-clang \
    ../../lib/async-clang \
    ../../lib/tztype-clang

HEADERS += \
    ../../tzaccess.h \
    ../../tzaccessapply.h \
    ../../tzaccessconfig.h \
    ../../tzaccessconn.h \
    ../../tzaccessparsecmp.h \
    ../../tzaccessstandardlayer.h \
    ../../lib/async-clang/async.h \
    ../../lib/crc16-clang/crc16.h \
    ../../lib/knock-clang/knock.h \
    ../../lib/lagan-clang/lagan.h \
    ../../lib/pt/lc.h \
    ../../lib/pt/lc-switch.h \
    ../../lib/pt/pt.h \
    ../../lib/pt/pt-sem.h \
    ../../lib/tzbox/tzbox.h \
    ../../lib/tzfifo/tzfifo.h \
    ../../lib/tzlist/tzlist.h \
    ../../lib/tzmalloc/bget.h \
    ../../lib/tzmalloc/tzmalloc.h \
    ../../lib/tztime/tztime.h \
    ../../lib/tztype-clang/tztype.h \
    ../../lib/utz-clang/utz.h \
    ../../lib/utz-clang/utzcmp.h \
    ../../lib/utz-clang/utzcommon.h \
    ../../lib/utz-clang/utzehp.h \
    ../../lib/utz-clang/utzflp.h \
    ../../lib/utz-clang/utznlv1.h
