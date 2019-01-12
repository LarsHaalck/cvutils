set(ftCompSources
    main.cpp
    featureDetector.cpp
)

set(ftHeaders
    featureDetector.h
)


find_package(cxxopts REQUIRED)
add_executable(ftComp ${ftCompSources})
target_include_directories(ftComp PUBLIC ${ftCompHeaders})
target_link_libraries(ftComp
    cxxopts::cxxopts
    opencv_core
    opencv_features2d
    opencv_xfeatures2d
    opencv_highgui
    opencv_imgproc
    opencv_imgcodecs
    stdc++fs
)
