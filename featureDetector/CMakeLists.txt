set(ftDetectSources
    main.cpp
    featureDetector.cpp
)

add_executable(ftDetect ${ftDetectSources})
target_include_directories(ftDetect PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(ftDetect
    cxxopts
    progress
    opencv_core
    opencv_features2d
    opencv_xfeatures2d
    opencv_imgproc
    opencv_imgcodecs
    stdc++fs
    io
)

if(OpenMP_FOUND)
    target_link_libraries(ftDetect OpenMP::OpenMP_CXX)
endif()
install(TARGETS ftDetect DESTINATION bin)
