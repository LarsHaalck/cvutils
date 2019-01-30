set(ftMatchSources
    main.cpp
    featureMatcher.cpp
)

add_executable(ftMatch ${ftMatchSources})
target_include_directories(ftMatch PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(ftMatch
    cxxopts
    opencv_core
    opencv_features2d
    opencv_xfeatures2d
    opencv_highgui
    opencv_imgproc
    opencv_imgcodecs
    opencv_calib3d
    stdc++fs
    io
)

install(TARGETS ftMatch DESTINATION bin)
