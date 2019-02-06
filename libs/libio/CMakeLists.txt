set(targetName io)

set(sources
    src/imageReader.cpp
    src/featureReader.cpp
    src/featureWriter.cpp
    src/descriptorReader.cpp
    src/descriptorWriter.cpp
)


#add_library(${targetName} STATIC
#    io/cache/abstractCache.h
#    io/cache/cacheFactory.h
#    io/cache/lruCache.h
#    io/cache/nonCache.h
#    io/cache/simpleCache.h
#    io/fetch/abstractFetcher.h
#    io/fetch/imageFetcher.h
#    io/imageReader.cpp
#    io/featureWriter.cpp
#)

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

