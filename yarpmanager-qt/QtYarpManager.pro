#-------------------------------------------------
#
# Project created by QtCreator 2014-02-03T09:21:15
#
#-------------------------------------------------

QT       += core gui widgets opengl

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = QtYARPManager
TEMPLATE = app

linux-g++{
    INCLUDEPATH += /home/apaikan/Install/robotology/yarp-apaikan/src/libYARP_OS/include
    INCLUDEPATH += /home/apaikan/Install/robotology/yarp-apaikan/libYARP_dev/include
    INCLUDEPATH += /home/apaikan/Install/robotology/yarp-apaikan/build/generated_include/
    INCLUDEPATH += /home/apaikan/Install/robotology/yarp-apaikan/src/libYARP_manager/include
    INCLUDEPATH += ../YarpBuilderLib

    CONFIG(debug, debug|release) {
        LIBS += -L/home/apaikan/Install/robotology/yarp-apaikan/build/lib
        LIBS += -L"$$(ACE_ROOT)/lib"
        LIBS += -L"$$(GSL_DIR)/lib"
        LIBS += -L/home/apaikan/Install/robotology/yarpmanager2/build-YarpBuilderLib-Desktop_Qt_5_2_1_GCC_64bit-Debug

        LIBS += -lYARP_init
        LIBS += -lYARP_manager
        LIBS += -ltinyxml
        LIBS += -lYARP_math
        LIBS += -lYARP_sig
        LIBS += -lYARP_OS
        LIBS += -lYarpBuilderLib

        LIBS += -lgsl
        LIBS += -lgslcblas
        #LIBS += -lyarpcar
        #LIBS += -lYARP_wire_rep_utilsd
        #LIBS += -lyarp_tcprosd
        #LIBS += -lyarp_bayer
        #LIBS += -lyarp_humand
        LIBS += -lACE

        deps_libs.files += /home/apaikan/Install/robotology/yarpmanager2/build-YarpBuilderLib-Desktop_Qt_5_2_1_GCC_64bit-Debug/libYarpBuilderLib.so



    }else{
        LIBS += -L$$(YARP_ROOT)/build/lib/Release
        LIBS += -L"$$(ACE_ROOT)/lib"
        LIBS += -L"$$(GSL_DIR)/lib"
        LIBS += -L../build-YarpBuilderLib-Desktop_Qt_5_3_MSVC2010_OpenGL_32bit-Release/release

        LIBS += -lYARP_math
        LIBS += -lYARP_manager
        LIBS += -lYARP_priv_tinyxml
        LIBS += -lYARP_OS
        LIBS += -lYARP_sig
        LIBS += -lYARP_init
        LIBS += -lgsl
        LIBS += -lgslcblas
        LIBS += -lyarpcar
        LIBS += -lyarp_bayer
        LIBS += -lACE
        LIBS += -lWinmm
        LIBS += -lAdvapi32
        LIBS += -lShell32
        LIBS += -lYarpBuilderLib
    }

        deps_libs.path = $$OUT_PWD/debug

        INSTALLS = deps_libs

}

win32-msvc2010{
    INCLUDEPATH += $$(YARP_ROOT)/src/libYARP_OS/include
    INCLUDEPATH += $$(YARP_ROOT)/src/libYARP_dev/include
    INCLUDEPATH += $$(YARP_ROOT)/build/generated_include/
    INCLUDEPATH += $$(YARP_ROOT)/src/yarpmanager/libymanager/include
    INCLUDEPATH += $$(YARP_ROOT)/src/libYARP_manager/include
    INCLUDEPATH += ../YarpBuilderLib

    CONFIG(debug, debug|release) {
        LIBS += -L$$(YARP_ROOT)/build/lib/Debug
        LIBS += -L"$$(ACE_ROOT)/lib"
        LIBS += -L"$$(GSL_DIR)/lib"
        LIBS += -L../build-YarpBuilderLib-Desktop_Qt_5_3_MSVC2010_OpenGL_32bit-Debug/debug

        LIBS += -lYARP_initd
        LIBS += -lYARP_managerd
        LIBS += -lYARP_priv_tinyxmld
        LIBS += -lYARP_mathd
        LIBS += -lYARP_sigd
        LIBS += -lYARP_OSd
        LIBS += -lYarpBuilderLib

        LIBS += -lgsl
        LIBS += -lgslcblas
        LIBS += -lyarpcard
        #LIBS += -lYARP_wire_rep_utilsd
        #LIBS += -lyarp_tcprosd
        LIBS += -lyarp_bayerd
        #LIBS += -lyarp_humand
        LIBS += -lACEd
        LIBS += -lWinmm
        LIBS += -lAdvapi32
        LIBS += -lShell32
        deps_libs.files += $$(YARP_ROOT)/src/build-YarpBuilderLib-Desktop_Qt_5_3_MSVC2010_OpenGL_32bit-Debug/debug/YarpBuilderLib.dll



    }else{
        LIBS += -L$$(YARP_ROOT)/build/lib/Release
        LIBS += -L"$$(ACE_ROOT)/lib"
        LIBS += -L"$$(GSL_DIR)/lib"
        LIBS += -L../build-YarpBuilderLib-Desktop_Qt_5_3_MSVC2010_OpenGL_32bit-Release/release

        LIBS += -lYARP_math
        LIBS += -lYARP_manager
        LIBS += -lYARP_priv_tinyxml
        LIBS += -lYARP_OS
        LIBS += -lYARP_sig
        LIBS += -lYARP_init
        LIBS += -lgsl
        LIBS += -lgslcblas
        LIBS += -lyarpcar
        LIBS += -lyarp_bayer
        LIBS += -lACE
        LIBS += -lWinmm
        LIBS += -lAdvapi32
        LIBS += -lShell32
        LIBS += -lYarpBuilderLib
    }

        deps_libs.path = $$OUT_PWD/debug

        INSTALLS = deps_libs

}

SOURCES += main.cpp\
        mainwindow.cpp \
    entitiestreewidget.cpp \
    moduleviewwidget.cpp \
    applicationviewwidget.cpp \
    safe_manager.cpp \
    genericviewwidget.cpp \
    customtreewidget.cpp \
    resourceviewwidget.cpp \
    yscopewindow.cpp \
    logwidget.cpp \
    stdoutwindow.cpp \
    aboutdlg.cpp \
    genericinfodlg.cpp \
    newapplicationwizard.cpp

HEADERS  += mainwindow.h \
    entitiestreewidget.h \
    moduleviewwidget.h \
    applicationviewwidget.h \
    safe_manager.h \
    genericviewwidget.h \
    customtreewidget.h \
    template_res.h \
    resourceviewwidget.h \
    yscopewindow.h \
    logwidget.h \
    stdoutwindow.h \
    aboutdlg.h \
    genericinfodlg.h \
    newapplicationwizard.h

FORMS    += mainwindow.ui \
    moduleviewwidget.ui \
    applicationviewwidget.ui \
    resourceviewwidget.ui \
    yscopewindow.ui \
    stdoutwindow.ui \
    aboutdlg.ui \
    genericinfodlg.ui

RESOURCES += \
    res.qrc
