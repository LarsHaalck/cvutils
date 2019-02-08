set(targetName qZoom)
add_library(${targetName} STATIC src/qGraphicsZoom.cpp include/zoom/qGraphicsZoom.h)
set_target_properties(${targetName} PROPERTIES AUTOMOC ON)
target_include_directories(${targetName} PUBLIC include)
target_link_libraries(${targetName}
    PUBLIC
    Qt5::Widgets
)

