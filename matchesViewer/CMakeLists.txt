# This file is part of OpenMVG (Open Multiple View Geometry) C++ library.
# Copyright (c) 2017 <Zillow Inc.> Pierre Moulon
# Modified by Lars Haalck

set(matchViewerSources
    main.cpp
    mainwindow.cpp
    mainframe.cpp
    pairgraphicsitem.cpp
    matchingpairgraphicsview.cpp
    document.cpp
)

add_executable(matchViewer ${matchViewerSources})
target_include_directories(matchViewer PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(matchViewer
    opencv_core
    opencv_features2d
    opencv_xfeatures2d
    opencv_highgui
    opencv_imgproc
    opencv_imgcodecs
    stdc++fs
    Qt5::Widgets
    io
    qImgCv
    qZoom
)

find_package(Qt5OpenGL)
if (Qt5OpenGL_FOUND)
    target_link_libraries(matchViewer Qt5::OpenGL)
else()
    add_definitions(-DQT_NO_OPENGL)
endif()

