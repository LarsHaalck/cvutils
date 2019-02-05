set(targetName qImgCv)
add_library(${targetName} STATIC src/qImgCv.cpp)
target_include_directories(${targetName} PUBLIC include)
target_link_libraries(${targetName}
    PUBLIC
    opencv_core
    opencv_imgproc
    Qt5::Widgets
)
