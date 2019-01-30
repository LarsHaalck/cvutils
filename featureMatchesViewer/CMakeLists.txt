set(ftMatchViewerSources
    main.cpp
    mainwindow.cpp
    mainframe.cpp
    pairgraphicsitem.cpp
    matchingpairgraphicsview.cpp
    document.cpp
)

set(ftMatchViewerHeaders
    main.cpp
    mainwindow.h
    mainframe.h
    pairgraphicsitem.h
    matchingpairgraphicsview.h
    document.hpp
)

add_executable(ftMatchViewer ${ftMatchViewerSources} ${ftMatchViewerHeaders})
set_target_properties(ftMatchViewer PROPERTIES AUTOMOC ON)
target_include_directories(ftMatchViewer PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(ftMatchViewer
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
    target_link_libraries(ftMatchViewer Qt5::OpenGL)
else()
    target_compile_definitions(ftMatchViewer PUBLIC -DQT_NO_OPENGL)
endif()

install(TARGETS ftMatchViewer DESTINATION bin)

