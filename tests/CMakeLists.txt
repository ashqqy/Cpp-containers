project(tests LANGUAGES CXX)

include(${CMAKE_SOURCE_DIR}/cmake/add_container_test.cmake)

find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

add_subdirectory(vector)
