# Copyright (C) 2014  iCub Facility, Istituto Italiano di Tecnologia
# Author: Ali Paikan <ali.paikan@iit.it>
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

#cmake_minimum_required(VERSION 2.8)
set(PROJECTNAME yarpmanager2)
project(${PROJECTNAME})

set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

include_directories(./ 
                    ../YarpBuilderLib
                    ${CMAKE_CURRENT_SOURCE_DIR}
                    ${YARP_INCLUDE_DIRS})

set(yarpmanager2_qt_SRCS 
    aboutdlg.cpp
    applicationviewwidget.cpp
    customtreewidget.cpp
    entitiestreewidget.cpp
    genericinfodlg.cpp
    genericviewwidget.cpp
    logwidget.cpp
    main.cpp
    mainwindow.cpp
    moduleviewwidget.cpp
    newapplicationwizard.cpp
    resourceviewwidget.cpp
    safe_manager.cpp
    stdoutwindow.cpp
    yscopewindow.cpp)

set(yarpmanager2_qt_HDRS 
    aboutdlg.h
    applicationviewwidget.h
    customtreewidget.h
    entitiestreewidget.h
    genericinfodlg.h
    genericviewwidget.h
    logwidget.h
    mainwindow.h
    moduleviewwidget.h
    newapplicationwizard.h
    resourceviewwidget.h
    safe_manager.h
    stdoutwindow.h
    template_res.h
    yscopewindow.h)

set(yarpmanager2_qt_QRC_FILES res.qrc)

set(yarpmanager2_qt_UI_FILES 
    aboutdlg.ui
    applicationviewwidget.ui
    genericinfodlg.ui
    mainwindow.ui
    moduleviewwidget.ui
    resourceviewwidget.ui
    stdoutwindow.ui
    yscopewindow.ui)


qt5_add_resources(yarpmanager2_qt_QRC_GEN_SRCS ${yarpmanager2_qt_QRC_FILES})
qt5_wrap_ui(yarpmanager2_qt_UI_GEN_SRCS ${yarpmanager2_qt_UI_FILES})

source_group("Source Files" FILES    ${yarpmanager2_qt_SRCS})
source_group("Header Files" FILES    ${yarpmanager2_qt_HDRS})
source_group("Resources Files" FILES ${yarpmanager2_qt_QRC_FILES})
source_group("UI Files" FILES        ${yarpmanager2_qt_UI_FILES})
source_group("Generated Files" FILES ${yarpmanager2_qt_QRC_GEN_SRCS}
                                     ${yarpmanager2_qt_UI_GEN_SRCS})


add_executable(yarpmanager2-qt WIN32 ${yarpmanager2_qt_SRCS}
                                      ${yarpmanager2_qt_HDRS}
                                      ${yarpmanager2_qt_QRC_FILES}
                                      ${yarpmanager2_qt_QRC_GEN_SRCS}
                                      ${yarpmanager2_qt_UI_GEN_SRCS}
                                      ${yarpmanager2_qt_UI_FILES})


target_link_libraries(yarpmanager2-qt ${YARP_LIBRARIES} YARP::YARP_manager yarpBuilder)
qtyarp_use_modules(yarpmanager2-qt Widgets Gui)

qtyarp_deprecate_with_cmake_version(2.8.11)
if(WIN32 AND CMAKE_VERSION VERSION_LESS 2.8.11)
    target_link_libraries(yarpmanager2-qt YARP::YARP_manager yarpBuilder Qt5::WinMain)
endif()

install(TARGETS yarpmanager2-qt COMPONENT utilities DESTINATION ${CMAKE_INSTALL_BINDIR})

#yarp_osx_duplicate_and_add_bundle(TARGET_ORIG yarpmanager2-qt
#                                TARGET_DEST yarpmanager2-qt.app
#                                APP_ICON Resources/AppIcon.icns
#                                INSTALL_COMPONENT utilities
#                                INSTALL_DESTINATION "${CMAKE_INSTALL_BINDIR}")



