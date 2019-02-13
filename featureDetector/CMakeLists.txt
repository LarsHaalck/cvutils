set(targetName ftDetect)

set(ftDetectSources
    main.cpp
    featureDetector.cpp
)

add_executable(${targetName} ${ftDetectSources})
target_include_directories(${targetName} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(${targetName}
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
    target_link_libraries(${targetName} OpenMP::OpenMP_CXX)
endif()
install(TARGETS ${targetName} DESTINATION bin)
