project(openCV-Util)
cmake_minimum_required(VERSION 3.5)

set(CMAKE_AUTOMOC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic -Wextra")

find_package(OpenCV REQUIRED)
find_package(cxxopts REQUIRED)
find_package(Qt5Widgets REQUIRED)


add_library(io STATIC misc/io.cpp)
target_include_directories(io PUBLIC misc)
target_link_libraries(io
    opencv_core
    stdc++fs
)

add_library(qImgCv STATIC misc/qImgCv.cpp)
target_include_directories(qImgCv PUBLIC misc)
target_link_libraries(qImgCv
    opencv_core
    opencv_imgproc
    Qt5::Widgets
)

add_library(qZoom STATIC misc/qGraphicsZoom.cpp)
target_include_directories(qZoom PUBLIC misc)
target_link_libraries(qZoom
    Qt5::Widgets
)

add_subdirectory(featureDetector)
add_subdirectory(featureViewer)
add_subdirectory(featureMatcher)
add_subdirectory(matchesViewer)
