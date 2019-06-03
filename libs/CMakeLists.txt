add_library(tqdm INTERFACE)
target_include_directories(tqdm INTERFACE external/cpptqdm)

add_library(cxxopts INTERFACE)
target_include_directories(cxxopts INTERFACE external/cxxopts/include)

add_subdirectory(external/cereal)
install(TARGETS cereal EXPORT cvutils-export)
# add_library(cereal INTERFACE)
# target_include_directories(cereal INTERFACE external/cereal/include)

add_subdirectory(libio)
add_subdirectory(libqimgcv)
add_subdirectory(libzoom)
add_subdirectory(libiso)
