project(openCV-Util)
cmake_minimum_required(3.5)


set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic -Wextra")
set(CXX_STANDARD 14)

# TODO: maybe add option to exclude non-free
find_package(OpenCV REQUIRED)
find_package(cxxopts REQUIRED)


add_subdirectory(featureComputation)
