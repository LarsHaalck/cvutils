project(cvutils)
cmake_minimum_required(VERSION 3.8)
set(VERSION 0.1)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic -Wextra")

# only for debug builds
# medium.com/@alasher/colored-c-compiler-output-with-ninja-clang-gcc-10bfe7f2b949
option (FORCE_COLORED_OUTPUT "Always produce ANSI-colored output (GNU/Clang only)." TRUE)
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

add_subdirectory(libs)
add_subdirectory(featureDetector)
add_subdirectory(featureViewer)
add_subdirectory(featureMatcher)
add_subdirectory(featureMatchesViewer)


# install all exported targets
include(CMakePackageConfigHelpers)
set(versionConfig "${PROJECT_BINARY_DIR}/cvutils-config-version.cmake")
set(packageConfig "${PROJECT_BINARY_DIR}/cvutils-config.cmake")

write_basic_package_version_file(
    ${versionConfig} 
    VERSION ${VERSION}
    COMPATIBILITY AnyNewerVersion
)

configure_package_config_file(
    ${PROJECT_SOURCE_DIR}/cvutils-config.cmake.in
    ${packageConfig} 
    INSTALL_DESTINATION cmake
)

install(FILES ${versionConfig} ${packageConfig}
    DESTINATION
    cmake
)

install(EXPORT cvutils-export
  FILE
    cvutilsTargets.cmake
  NAMESPACE
    cvutils::
  DESTINATION
    cmake
)

