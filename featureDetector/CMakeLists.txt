set(ftDetectSources
    main.cpp
    featureDetector.cpp
)

add_executable(ftDetect ${ftDetectSources})
target_include_directories(ftDetect PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(ftDetect
    cxxopts::cxxopts
    opencv_core
    opencv_features2d
    opencv_xfeatures2d
    opencv_highgui
    opencv_imgproc
    opencv_imgcodecs
    stdc++fs
    io
)
