cmake_minimum_required(VERSION 3.13)

project(unit_test C CXX)

# Enable testing
enable_testing()

# Define the path to your STM32 toolchain file
set(CMAKE_TOOLCHAIN_FILE ${CMAKE_SOURCE_DIR}/cmake/GCCNativeToolchain.cmake)

set(MAIN_PROJ_DIR ../../)

# Include directories
include_directories(
    ../Inc
    mocks
    ${MAIN_PROJ_DIR}third_party/googletest/googletest/include
)

# Add source files
set(SOURCES
    ../Src/myTest.c
    cmake_src/test_main.cpp
)

# Create the executable
add_executable(${PROJECT_NAME} ${SOURCES})

target_link_directories(${PROJECT_NAME} PUBLIC ../../third_party/googletest/build/lib)

# Link the necessary libraries
target_link_libraries(${PROJECT_NAME} gtest gtest_main pthread)

# Define the tests
file(GLOB TEST_SOURCES "*.cpp")

foreach(test_src ${TEST_SOURCES})
    get_filename_component(test_name ${test_src} NAME_WE)
    add_executable(${test_name} ${test_src})
    target_link_libraries(${test_name} gtest gtest_main)
    add_test(NAME ${test_name} COMMAND ${test_name})
endforeach()