set(targetName ftView)

set(Sources
    main.cpp
    mainWindow.cpp
)

set(Headers
    mainWindow.h
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

install(TARGETS ${targetName} DESTINATION bin)

