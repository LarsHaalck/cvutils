set(targetName ftMatch)
set(Sources
    main.cpp
    featureMatcher.cpp
)

add_executable(${targetName} ${Sources})
add_executable(${targetName}2 extract_pairs.cpp)

target_include_directories(${targetName} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(${targetName}
    PRIVATE
    cxxopts
    tqdm
    opencv_core
    opencv_features2d
    opencv_xfeatures2d
    opencv_highgui
    opencv_imgproc
    opencv_imgcodecs
    opencv_calib3d
    stdc++fs
    io
    iso
)

target_link_libraries(${targetName}2
    PRIVATE
    opencv_core
    opencv_features2d
    io
)

if(OpenMP_FOUND)
    target_link_libraries(${targetName} PRIVATE OpenMP::OpenMP_CXX)
endif()

install(TARGETS ${targetName} DESTINATION bin)
install(TARGETS ${targetName}2 DESTINATION bin)
