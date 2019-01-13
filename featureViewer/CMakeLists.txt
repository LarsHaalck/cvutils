set(ftViewSources
    main.cpp
    mainWindow.cpp
)

# !!!AUTOMOC DOES NOT WORK WHEN APPLYING IT ON A SINGLE TARGET!!!
set(CMAKE_AUTOMOC ON)
add_executable(ftView ${ftViewSources})
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
