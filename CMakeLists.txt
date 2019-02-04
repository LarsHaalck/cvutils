project(openCV-Util)
cmake_minimum_required(VERSION 3.8)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic -Wextra")

# only for debug builds
# medium.com/@alasher/colored-c-compiler-output-with-ninja-clang-gcc-10bfe7f2b949
option (FORCE_COLORED_OUTPUT "Always produce ANSI-colored output (GNU/Clang only)." FALSE)
if (${FORCE_COLORED_OUTPUT})
    if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
       add_compile_options (-fdiagnostics-color=always)
    elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
       add_compile_options (-fcolor-diagnostics)
    endif ()
endif ()

find_package(OpenCV REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(OpenMP QUIET)

if(NOT OpenMP_FOUND)
    message(WARNING "OpenMP not found, compiling without...")
else()
    message(STATUS "Building with OpenMP...")
endif()

# general libraries needed by multiple submodules
add_library(cxxopts INTERFACE)
target_include_directories(cxxopts INTERFACE misc/cxxopts)

add_library(progress INTERFACE)
target_include_directories(progress INTERFACE misc/tqdm)

add_library(io STATIC misc/io/io.cpp)
target_include_directories(io PUBLIC misc/io)
target_link_libraries(io
    opencv_core
    stdc++fs
    progress
)

if(OpenMP_FOUND)
    target_link_libraries(io OpenMP::OpenMP_CXX)
endif()

add_library(qImgCv STATIC misc/qimgcv/qImgCv.cpp)
target_include_directories(qImgCv PUBLIC misc/qimgcv)
target_link_libraries(qImgCv
    PUBLIC
    opencv_core
    opencv_imgproc
    Qt5::Widgets
)

add_library(qZoom STATIC misc/zoom/qGraphicsZoom.cpp)
set_target_properties(qZoom PROPERTIES AUTOMOC ON)
target_include_directories(qZoom PUBLIC misc/zoom)
target_link_libraries(qZoom
    PUBLIC
    Qt5::Widgets
)


add_subdirectory(featureDetector)
add_subdirectory(featureViewer)
add_subdirectory(featureMatcher)
add_subdirectory(featureMatchesViewer)
