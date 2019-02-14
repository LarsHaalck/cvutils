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
target_include_directories(${targetName}
    PUBLIC
    $<INSTALL_INTERFACE:include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)
target_link_libraries(${targetName}
    PRIVATE
    opencv_core
    opencv_imgproc
    opencv_imgcodecs
    opencv_features2d
    opencv_xfeatures2d
    stdc++fs
)

if(OpenMP_FOUND)
    target_link_libraries(${targetName} PRIVATE OpenMP::OpenMP_CXX)
endif()

install(TARGETS ${targetName}
    EXPORT cvutils-export
    DESTINATION ${targetName}
)

# set(cacheHeaders
#     include/cache/abstractCache.h
#     include/cache/cacheFactory.h
#     include/cache/hash.h
#     include/cache/lruCache.h
#     include/cache/nonCache.h
#     include/cache/simpleCache.h
# )

# set(fetchHeaders
#     include/fetch/abstractFetcher.h
#     include/fetch/descriptorFetcher.h
#     include/fetch/featureFetcher.h
#     include/fetch/imageFetcher.h
#     include/fetch/matchesFetcher.h
# )

# set(ioHeaders
#     include/io/config.h
#     include/io/descriptorReader.h
#     include/io/descriptorWriter.h
#     include/io/featureReader.h
#     include/io/featureWriter.h
#     include/io/imageReader.h
#     include/io/matchType.h
#     include/io/matchesReader.h
#     include/io/matchesWriter.h
# )

install(DIRECTORY include/cache  DESTINATION include/cache)
install(DIRECTORY include/fetch DESTINATION include/fetch)
install(DIRECTORY include/io DESTINATION include/io)


