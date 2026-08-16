cmake_minimum_required(VERSION 3.20)

project(atlas-routing VERSION 0.1.0 LANGUAGES CXX)

include(CTest)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(atlas_core
    src/atlas/core.cpp
    src/atlas/graph.cpp
    src/atlas/graph_io.cpp
    src/atlas/traversal.cpp
)
target_include_directories(atlas_core PUBLIC include)
target_compile_features(atlas_core PUBLIC cxx_std_20)

if (MSVC)
    target_compile_options(atlas_core PRIVATE /W4 /permissive-)
else()
    target_compile_options(atlas_core PRIVATE -Wall -Wextra -Wpedantic)
endif()

add_executable(atlas apps/atlas_main.cpp)
target_link_libraries(atlas PRIVATE atlas_core)

if (BUILD_TESTING)
    add_executable(atlas_core_tests tests/core_tests.cpp)
    target_link_libraries(atlas_core_tests PRIVATE atlas_core)
    add_test(NAME atlas_core_tests COMMAND atlas_core_tests)
endif()
