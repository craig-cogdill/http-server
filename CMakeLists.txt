# CMakeLists files in this project can
# refer to the root source directory of the project as ${HELLO_SOURCE_DIR} and
# to the root binary directory of the project as ${HELLO_BINARY_DIR}.
cmake_minimum_required (VERSION 3.0)
project (http-server)
set (CMAKE_CXX_STANDARD 14)
set (CMAKE_CXX_FLAGS "-Wall -Wunused -Werror")

set (SRC_DIR ${http-server_SOURCE_DIR}/src)
set (TEST_DIR ${http-server_SOURCE_DIR}/test)
include_directories (${SRC_DIR})

file (GLOB SRC_FILES ${SRC_DIR}/*.cpp ${SRC_DIR}/*.h)
file (GLOB TEST_FILES ${TEST_DIR}/*.cpp ${TEST_DIR}/*.h ${TEST_DIR}/*.hpp)

add_executable (unit-test-runner ${TEST_FILES})
add_executable (http-server ${SRC_FILES})