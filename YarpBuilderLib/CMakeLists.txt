# Copyright (C) 2014  iCub Facility, Istituto Italiano di Tecnologia
# Author: Ali Paikan <ali.paikan@iit.it>
# CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT

# all files which may be mocced
set(libYarpBuilder_HDRS
    applicationitem.h
    arrow.h
    builderitem.h
    builderscene.h
    builderwindow.h
    commons.h
    customtree.h
    destinationportitem.h
    itemsignalhandler.h
    moduleitem.h
    propertiestable.h
    sourceportitem.h
    yarpbuilderlib_global.h
    yarpbuilderlib.h )


set(libYarpBuilder_SRCS
    applicationitem.cpp
    arrow.cpp
    builderitem.cpp
    builderscene.cpp
    builderwindow.cpp
    customtree.cpp
    destinationportitem.cpp
    itemsignalhandler.cpp
    moduleitem.cpp
    propertiestable.cpp
    sourceportitem.cpp
    yarpbuilderlib.cpp)



set(libYarpBuilder_QRC_FILES res.qrc)
#set(libYarpBuilder_UI_FILES main.ui)


# include with QT_USE selected library parts
#INCLUDE(${QT_USE_FILE})

set(CMAKE_INCLUDE_CURRENT_DIR ON)

include_directories(
    ${YARP_INCLUDE_DIRS}
    ${Qt5Widgets_INCLUDE_DIRS}
	${CMAKE_CURRENT_SOURCE_DIR}
    ../yarpmanager-qt)

# Activate Export macro for building the library itself
#ADD_DEFINITIONS(-D...)

qt5_add_resources(libYarpBuilder_QRC_GEN_SRCS ${libYarpBuilder_QRC_FILES})
#qt5_wrap_ui(libYarpBuilder_UI_GEN_SRCS ${libYarpBuilder_UI_FILES})

source_group("Source Files" FILES    ${libYarpBuilder_SRCS})
source_group("Header Files" FILES    ${libYarpBuilder_HDRS})
source_group("Resources Files" FILES ${libYarpBuilder_QRC_FILES})
source_group("Generated Files" FILES ${libYarpBuilder_QRC_GEN_SRCS})

add_library(yarpBuilder SHARED
            ${libYarpBuilder_SRCS}
            ${libYarpBuilder_HDRS} 
            ${libYarpBuilder_QRC_FILES}
            ${libYarpBuilder_QRC_GEN_SRCS})

target_link_libraries(yarpBuilder
                      ${QT_LIBRARIES}
                 	  ${YARP_LIBRARIES}
                      YARP::YARP_manager)

#qtyarp_use_modules(qgvcore Core Widgets)
#qt5_use_modules(qgvcore Core Widgets)

