cmake_minimum_required(VERSION 3.11.0)
project(validate-1euro VERSION 0.1.0 LANGUAGES C CXX)

set(EXTERNAL_INSTALL_LOCATION ${CMAKE_BINARY_DIR}/external)

include(FetchContent)
FetchContent_Declare(matplotplusplus
        GIT_REPOSITORY https://github.com/alandefreitas/matplotplusplus
        GIT_TAG origin/master) # or whatever tag you want

FetchContent_GetProperties(matplotplusplus)
if(NOT matplotplusplus_POPULATED)
    FetchContent_Populate(matplotplusplus)
    add_subdirectory(${matplotplusplus_SOURCE_DIR} ${matplotplusplus_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()

set (CMAKE_CXX_STANDARD 17)

add_executable(validate-1euro main.cpp FilteredDistance.cpp FilteredDistanceKalman.cpp)
target_link_libraries(validate-1euro PUBLIC matplot)