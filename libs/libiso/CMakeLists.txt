set(targetName iso)
add_library(${targetName} STATIC src/isometry.cpp src/ransac.cpp)
target_include_directories(${targetName} PUBLIC include)
target_link_libraries(${targetName}
    PUBLIC
    opencv_core
    opencv_calib3d
)
