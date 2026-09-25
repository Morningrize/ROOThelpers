cmake_minimum_required(VERSION 3.10)
project(ROOThelpers LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(CMakePackageConfigHelpers)

option(ANALYSISCORE_FETCH_IF_MISSING "Fetch AnalysisCoreCpp from git when package is not found" OFF)
option(ENABLE_CLANG_TIDY "Enable clang-tidy" OFF)

# ----------------------
# Dependencies
# ----------------------

# AnalysisCore

if(DEFINED ENV{AnalysisCoreCpp_DIR})
  list(PREPEND CMAKE_PREFIX_PATH "$ENV{AnalysisCoreCpp_DIR}")
endif()

if(DEFINED ENV{cxxopts_DIR})
    list(PREPEND CMAKE_PREFIX_PATH "$ENV{cxxopts_DIR}")
endif()

find_package(AnalysisCoreCpp CONFIG QUIET)

set(ANALYSISCORE_TARGET "")

if(AnalysisCoreCpp_FOUND)
    if(TARGET AnalysisCoreCpp::AnalysisCoreCpp)
        set(ANALYSISCORE_TARGET AnalysisCoreCpp::AnalysisCoreCpp)
    elseif(TARGET AnalysisCoreCpp)
        set(ANALYSISCORE_TARGET AnalysisCoreCpp)
    else()
        message(FATAL_ERROR "AnalysisCoreCpp package was found, but no usable target was exported (expected AnalysisCoreCpp::AnalysisCoreCpp or AnalysisCoreCpp).")
    endif()
    message(STATUS "Using AnalysisCoreCpp package target: ${ANALYSISCORE_TARGET}")
elseif (ANALYSISCORE_FETCH_IF_MISSING)
    message(STATUS "Fetching AnalysisCoreCpp")
    include(FetchContent)
    FetchContent_Declare(
        AnalysisCoreCpp
        GIT_REPOSITORY https://github.com/Morningrize/AnalysisCoreCpp.git
        GIT_TAG        master
    )
    FetchContent_MakeAvailable(AnalysisCoreCpp)
    if(TARGET AnalysisCoreCpp::AnalysisCoreCpp)
        set(ANALYSISCORE_TARGET AnalysisCoreCpp::AnalysisCoreCpp)
    elseif(TARGET AnalysisCoreCpp)
        set(ANALYSISCORE_TARGET AnalysisCoreCpp)
    else()
        message(FATAL_ERROR "Fetched AnalysisCoreCpp, but no usable target was created (expected AnalysisCoreCpp::AnalysisCoreCpp or AnalysisCoreCpp).")
    endif()
else()
    message(FATAL_ERROR "AnalysisCoreCpp package not found. Set AnalysisCoreCpp_DIR to a valid package directory (or export AnalysisCoreCpp_DIR), or enable ANALYSISCORE_FETCH_IF_MISSING=ON.")
endif()

# ROOT
find_package(ROOT REQUIRED)

# GTest
find_package(GTest QUIET)

# ----------------------
# Library
# ----------------------

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cxx)

add_library(ROOThelpers SHARED ${SOURCES})
add_library(ROOThelpers::ROOThelpers ALIAS ROOThelpers)

target_include_directories(ROOThelpers 
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        ${ROOT_INCLUDE_DIRS}
)
target_link_libraries(ROOThelpers 
    PUBLIC ${ANALYSISCORE_TARGET}
    PRIVATE ${ROOT_LIBRARIES})

# Export a build-tree package config for find_package(ROOThelpers CONFIG)
set(ROOTHELPERS_PACKAGE_DIR "${CMAKE_CURRENT_BINARY_DIR}/cmake")
set(ROOTHELPERS_CONFIG_FILE "${ROOTHELPERS_PACKAGE_DIR}/ROOThelpersConfig.cmake")
set(ROOTHELPERS_TARGETS_FILE "${ROOTHELPERS_PACKAGE_DIR}/ROOThelpersTargets.cmake")

export(
    TARGETS ROOThelpers
    FILE "${ROOTHELPERS_TARGETS_FILE}"
    NAMESPACE ROOThelpers::
)

configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/ROOThelpersConfig.cmake.in"
    "${ROOTHELPERS_CONFIG_FILE}"
    INSTALL_DESTINATION "cmake"
)

# ----------------------
# Tests
# ----------------------

if (GTest_FOUND)
    message("GTest found")
    enable_testing()
    include(GoogleTest)

    file(GLOB TEST_SOURCES CONFIGURE_DEPENDS tests/test_*.cpp)

    foreach(test_src ${TEST_SOURCES})
        get_filename_component(test_name ${test_src} NAME_WE)
        add_executable(${test_name} EXCLUDE_FROM_ALL ${test_src})
        target_include_directories(${test_name} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
        target_link_libraries(${test_name}
            PRIVATE 
                ROOThelpers
                GTest::gtest
                GTest::gtest_main
                ${ROOT_LIBRARIES}
        )
        gtest_discover_tests(${test_name})
        list(APPEND ALL_TEST_TARGETS ${test_name})   
    endforeach()
    
    add_custom_target(ROOThelpers_tests DEPENDS ${ALL_TEST_TARGETS})
else()
    message(WARNING "GTest not found. Tests will not be built. Install GTest to enable tests.")
endif()

# ----------------------
# Clang-Tidy
# ----------------------

if(ENABLE_CLANG_TIDY)
    find_program(CLANG_TIDY clang-tidy)
    if(CLANG_TIDY)
        file(GLOB_RECURSE ALL_SOURCE_FILES
            ${CMAKE_SOURCE_DIR}/include/*.h
            ${CMAKE_SOURCE_DIR}/src/*.cpp
            ${CMAKE_SOURCE_DIR}/tests/*.cpp
        )
        add_custom_target(tidy
            COMMAND ${CLANG_TIDY}
                -p ${CMAKE_BINARY_DIR}
                ${ALL_SOURCE_FILES}
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
            COMMENT "Running clang-tidy on all source files..."
        )
    else()
        message(WARNING "clang-tidy not found. 'tidy' target will not be available.")
    endif()
endif()