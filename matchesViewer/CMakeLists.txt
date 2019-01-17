set(matchViewerSources
    main.cpp
    mainwindow.cpp
    mainframe.cpp
    pairgraphicsitem.cpp
    matchingpairgraphicsview.cpp
    document.cpp
)

set(matchViewerHeaders
    main.cpp
    mainwindow.h
    mainframe.h
    pairgraphicsitem.h
    matchingpairgraphicsview.h
    document.hpp
)

add_executable(matchViewer ${matchViewerSources} ${matchViewerHeaders})
set_target_properties(matchViewer PROPERTIES AUTOMOC ON)
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
    target_compile_definitions(matchViewer PUBLIC -DQT_NO_OPENGL)
endif()

