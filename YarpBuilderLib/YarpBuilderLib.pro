#-------------------------------------------------
#
# Project created by QtCreator 2015-09-30T16:29:56
#
#-------------------------------------------------

QT       += widgets

TARGET = YarpBuilderLib
TEMPLATE = lib

DEFINES += YARPBUILDERLIB_LIBRARY



SOURCES += yarpbuilderlib.cpp \
    applicationitem.cpp \
    arrow.cpp \
    builderitem.cpp \
    builderscene.cpp \
    customtree.cpp \
    destinationportitem.cpp \
    itemsignalhandler.cpp \
    moduleitem.cpp \
    sourceportitem.cpp \
    builderwindow.cpp \
    propertiestable.cpp

HEADERS += yarpbuilderlib.h\
        yarpbuilderlib_global.h \
    applicationitem.h \
    arrow.h \
    builderitem.h \
    builderscene.h \
    commons.h \
    customtree.h \
    destinationportitem.h \
    itemsignalhandler.h \
    moduleitem.h \
    sourceportitem.h \
    ui_mainwindow.h \
    builderwindow.h \
    propertiestable.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}

RESOURCES += \
    res.qrc

DISTFILES += \
    YarpBuilderLib.pro.user

FORMS +=



win32-msvc2010{
    INCLUDEPATH += $$(YARP_ROOT)/src/libYARP_OS/include
    INCLUDEPATH += $$(YARP_ROOT)/src/libYARP_dev/include
    INCLUDEPATH += $$(YARP_ROOT)/build/generated_include/
    INCLUDEPATH += $$(YARP_ROOT)/src/yarpmanager/libymanager/include
    INCLUDEPATH += $$(YARP_ROOT)/src/libYARP_manager/include
    INCLUDEPATH += ../yarpmanager-qt

    CONFIG(debug, debug|release) {
        LIBS += -L$$(YARP_ROOT)/build/lib/Debug
            LIBS += -lYARP_initd
            LIBS += -lYARP_managerd
            LIBS += -lYARP_priv_tinyxmld
            LIBS += -lYARP_mathd
            LIBS += -lYARP_sigd
            LIBS += -lYARP_OSd
    }else{
        LIBS += -L$$(YARP_ROOT)/build/lib/Release
            LIBS += -lYARP_math
            LIBS += -lYARP_manager
            LIBS += -lYARP_priv_tinyxml
            LIBS += -lYARP_OS
            LIBS += -lYARP_sig
            LIBS += -lYARP_init
    }
}

linux-g++{
    INCLUDEPATH += /home/apaikan/Install/robotology/yarp-apaikan/src/libYARP_OS/include
    INCLUDEPATH += /home/apaikan/Install/robotology/yarp-apaikan/src/libYARP_dev/include
    INCLUDEPATH += /home/apaikan/Install/robotology/yarp-apaikan/build/generated_include/
    INCLUDEPATH += /home/apaikan/Install/robotology/yarp-apaikan/src/yarpmanager/libymanager/include
    INCLUDEPATH += /home/apaikan/Install/robotology/yarp-apaikan/src/libYARP_manager/include
    INCLUDEPATH += ../yarpmanager-qt

    CONFIG(debug, debug|release) {
        LIBS += -L/home/apaikan/Install/robotology/yarp-apaikan/build/lib
            LIBS += -lYARP_init
            LIBS += -lYARP_manager
            #LIBS += -lYARP_priv_tinyxml
            LIBS += -lYARP_math
            LIBS += -lYARP_sig
            LIBS += -lYARP_OS
    }else{
        LIBS += -L/home/apaikan/Install/robotology/yarp-apaikan/build/lib
            LIBS += -lYARP_math
            LIBS += -lYARP_manager
            #LIBS += -lYARP_priv_tinyxml
            LIBS += -lYARP_OS
            LIBS += -lYARP_sig
            LIBS += -lYARP_init
    }
}


