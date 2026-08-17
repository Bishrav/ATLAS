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
    src/atlas/shortest_path.cpp
    src/atlas/a_star.cpp
    src/atlas/bidirectional.cpp
    src/atlas/benchmark.cpp
    src/atlas/snapshot.cpp
    src/atlas/route_cache.cpp
    src/atlas/dynamic_update.cpp
    src/atlas/landmark.cpp
    src/atlas/vrp.cpp
    src/atlas/vrp_baseline.cpp
    src/atlas/vrp_evaluation.cpp
    src/atlas/vrp_optimization.cpp
    src/atlas/vrp_multi_vehicle.cpp
    src/atlas/vrp_comparison.cpp
    src/atlas/routing_service.cpp
    src/atlas/route_api.cpp
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

add_executable(atlas_benchmark apps/atlas_benchmark.cpp)
target_link_libraries(atlas_benchmark PRIVATE atlas_core)

if (BUILD_TESTING)
    add_executable(atlas_core_tests tests/core_tests.cpp)
    target_link_libraries(atlas_core_tests PRIVATE atlas_core)
    add_test(NAME atlas_core_tests COMMAND atlas_core_tests)
    add_test(NAME atlas_demo_route COMMAND atlas demo-route --algorithm dijkstra)
    add_test(NAME atlas_invalid_cli_arguments COMMAND atlas invalid-command)
    set_tests_properties(atlas_invalid_cli_arguments PROPERTIES WILL_FAIL TRUE)
endif()
