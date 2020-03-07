# CMakeLists files in this project can
# refer to the root source directory of the project as ${HELLO_SOURCE_DIR} and
# to the root binary directory of the project as ${HELLO_BINARY_DIR}.
cmake_minimum_required (VERSION 3.0)
project (http-server)
set (CMAKE_CXX_STANDARD 11)
set (CMAKE_CXX_FLAGS "-Wall -Wunused")

file (GLOB SOURCES "src/*.cpp")

add_executable (http-server ${SOURCES})
