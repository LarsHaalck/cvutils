set(targetName ftMatch)
set(ftMatchSources
    main.cpp
    featureMatcher.cpp
)

add_executable(${targetName} ${ftMatchSources})
target_include_directories(${targetName} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(${targetName}
    PRIVATE
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
    progress
)


if(OpenMP_FOUND)
    target_link_libraries(${targetName} PRIVATE OpenMP::OpenMP_CXX)
endif()
install(TARGETS ${targetName} DESTINATION bin)
