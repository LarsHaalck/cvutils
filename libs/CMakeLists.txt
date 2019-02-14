add_library(tqdm INTERFACE)
target_include_directories(tqdm INTERFACE external/cpptqdm)

add_library(cxxopts INTERFACE)
target_include_directories(cxxopts INTERFACE external/cxxopts/include)


if (NOT USE_SUPPLIED_CEREAL)
    find_package(Cereal)
    if (NOT Cereal_FOUND)
        message(FATAL_ERROR "Cereal was not found. Set the correct dir or set \
            USE_SUPPLIED_CEREAL to use the supplied submodule")
    endif()
    find_package(RapidJSON)
    if (NOT RapidJSON_FOUND)
        message(FATAL_ERROR "RapidJSON was not found. Set the hin RapidJSON_HINT or set \
            USE_SUPPLIED_CEREAL to use the supplied submodule")
    endif()
else()
    add_subdirectory(external/cereal)
endif()


add_subdirectory(libio)
add_subdirectory(libqimgcv)
add_subdirectory(libzoom)
