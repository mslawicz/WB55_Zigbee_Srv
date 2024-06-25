cmake_minimum_required(VERSION 3.13)

project(CoreTest C CXX)

# Define the path to your STM32 toolchain file
set(CMAKE_TOOLCHAIN_FILE ${CMAKE_SOURCE_DIR}/cmake/GCCNativeToolchain.cmake)

# Include directories
include_directories(../Inc)

# Add source files
set(SOURCES
    ../Src/myTest.c
)

# Create the executable
add_executable(${PROJECT_NAME} ${SOURCES})

# Link the necessary libraries
#target_link_libraries(${PROJECT_NAME} HAL_Driver)

# Enable testing
enable_testing()

# Add subdirectory for tests
#add_subdirectory(tests)

# Add googletest directly to our build. This defines
# the gtest and gtest_main targets.
#add_subdirectory(${CMAKE_BINARY_DIR}/googletest-src
#                 ${CMAKE_BINARY_DIR}/googletest-build
#                 EXCLUDE_FROM_ALL)

# Link against gtest or gtest_main as needed
include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})
include_directories(${CMAKE_SOURCE_DIR}/include)

# Define the tests
file(GLOB TEST_SOURCES "*.cpp")

foreach(test_src ${TEST_SOURCES})
    get_filename_component(test_name ${test_src} NAME_WE)
    add_executable(${test_name} ${test_src})
    target_link_libraries(${test_name} gtest gtest_main)
    add_test(NAME ${test_name} COMMAND ${test_name})
endforeach()