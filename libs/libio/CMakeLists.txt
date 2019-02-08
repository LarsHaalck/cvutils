set(targetName io)

set(sources
    src/imageReader.cpp
    src/featureReader.cpp
    src/featureWriter.cpp
    src/descriptorReader.cpp
    src/descriptorWriter.cpp
    src/matchesReader.cpp
    src/matchesWriter.cpp
    src/config.cpp
)


add_library(${targetName} STATIC ${sources})
target_include_directories(io PUBLIC include)
target_link_libraries(io
    opencv_core
    opencv_imgproc
    opencv_imgcodecs
    opencv_features2d
    opencv_xfeatures2d
    stdc++fs
    progress
)

if(OpenMP_FOUND)
    target_link_libraries(io OpenMP::OpenMP_CXX)
endif()

