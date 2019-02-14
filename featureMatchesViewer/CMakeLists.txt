set(targetName ftMatchViewer)
set(Sources
    main.cpp
    mainwindow.cpp
    mainframe.cpp
    pairgraphicsitem.cpp
    matchingpairgraphicsview.cpp
    document.cpp
)

set(Headers
    main.cpp
    mainwindow.h
    mainframe.h
    pairgraphicsitem.h
    matchingpairgraphicsview.h
    document.hpp
)

add_executable(${targetName} ${Sources} ${Headers})
set_target_properties(${targetName} PROPERTIES AUTOMOC ON)
target_include_directories(${targetName} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(${targetName}
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
    target_link_libraries(${targetName} Qt5::OpenGL)
else()
    target_compile_definitions(${targetName} PUBLIC -DQT_NO_OPENGL)
endif()

install(TARGETS ${targetName} DESTINATION bin)

