cmake_minimum_required(VERSION 3.15)
project(PowerFlowCalc LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Fetch nlohmann/json
include(FetchContent)
FetchContent_Declare(json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG v3.11.3
)
FetchContent_MakeAvailable(json)

# Fetch Eigen
FetchContent_Declare(Eigen
    GIT_REPOSITORY https://gitlab.com/libeigen/eigen.git
    GIT_TAG 3.4.0
)
FetchContent_MakeAvailable(Eigen)

# Compile options
add_compile_options(-Wall -Wextra -Wpedantic)

# Main executable
add_executable(powerflow_calc
    src/main.cpp
    src/data_loader.cpp
    src/network_builder.cpp
    src/power_flow_solver.cpp
    src/flow_calculator.cpp
    src/report_generator.cpp
    src/utils.cpp
)

target_include_directories(powerflow_calc PRIVATE
    ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(powerflow_calc PRIVATE
    nlohmann_json::nlohmann_json
    Eigen3::Eigen
)

# Tests
option(BUILD_TESTS "Build unit tests" ON)

if(BUILD_TESTS)
    enable_testing()

    FetchContent_Declare(googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG v1.14.0
    )
    FetchContent_MakeAvailable(googletest)

    add_executable(powerflow_tests
        tests/test_data_loader.cpp
        tests/test_network_builder.cpp
        tests/test_solver.cpp
        tests/test_flow_calculator.cpp
        tests/test_report_generator.cpp
        tests/test_utils.cpp
        src/data_loader.cpp
        src/network_builder.cpp
        src/power_flow_solver.cpp
        src/flow_calculator.cpp
        src/report_generator.cpp
        src/utils.cpp
    )

    target_include_directories(powerflow_tests PRIVATE
        ${CMAKE_SOURCE_DIR}/include
    )

    target_link_libraries(powerflow_tests PRIVATE
        nlohmann_json::nlohmann_json
        Eigen3::Eigen
        GTest::gtest_main
    )

    include(GoogleTest)
    gtest_discover_tests(powerflow_tests)
endif()
