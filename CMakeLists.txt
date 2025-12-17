cmake_minimum_required(VERSION 3.10)
project(ROOThelpers LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(ROOT REQUIRED)
include_directories(${ROOT_INCLUDE_DIRS})

add_library(ROOThelpers INTERFACE)
target_include_directories(ROOThelpers INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)
target_link_libraries(ROOThelpers INTERFACE ${ROOT_LIBRARIES})

find_package(GTest QUIET)

if (GTest_FOUND)
    message("GTest found")
    enable_testing()
    add_executable(test_axisstyle EXCLUDE_FROM_ALL tests/test_axisstyle.cpp)
    target_link_libraries(test_axisstyle PRIVATE ROOThelpers GTest::gtest GTest::gtest_main)
    add_test(NAME AxisStyleTest COMMAND test_axisstyle)
    add_custom_target(tests DEPENDS test_axisstyle)
else()
    message(WARNING "GTest not found. Tests will not be built. Install GTest to enable tests.")
endif()
