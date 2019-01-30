set(ftViewSources
    main.cpp
    mainWindow.cpp
)

set(ftViewHeaders
    mainWindow.h
)

add_executable(ftView ${ftViewSources} ${ftViewHeaders})
set_target_properties(ftView PROPERTIES AUTOMOC ON)
target_include_directories(ftView PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(ftView
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

install(TARGETS ftView DESTINATION bin)

