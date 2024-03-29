cmake_minimum_required (VERSION 3.0)
project (http-server)
set (CMAKE_CXX_STANDARD 11)
set (CMAKE_CXX_FLAGS "-Wall -Wunused -Werror")

set (SRC_DIR   ${http-server_SOURCE_DIR}/src)
set (TEST_DIR  ${http-server_SOURCE_DIR}/test)
set (GTEST_DIR ${http-server_SOURCE_DIR}/gtest/googletest-release-1.7.0)
include_directories (${SRC_DIR})

file (GLOB SRC_FILES ${SRC_DIR}/*.cpp ${SRC_DIR}/*.h)
add_executable (http-server ${SRC_FILES})

# THE FOLLOWING SECTION IS DIRECTLY FROM THE GOOGLETEST DOCUMENTATION,
# WHICH IS OPEN SOURCE. I DO NOT CLAIM IT AS MY OWN
#
# -- START SECTION -------------------------------------------------------------
# Create unit test executable
configure_file(gtest/CMakeLists.txt.in gtest/googletest-download/CMakeLists.txt)
execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
  RESULT_VARIABLE result
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/gtest/googletest-download ) 
     if(result)
  message(FATAL_ERROR "CMake step for googletest failed: ${result}")
endif()
execute_process(COMMAND ${CMAKE_COMMAND} --build .
  RESULT_VARIABLE result
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/gtest/googletest-download )
if(result)
  message(FATAL_ERROR "Build step for googletest failed: ${result}")
endif()

# Prevent overriding the parent project's compiler/linker
# settings on Windows
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

# Add googletest directly to our build. This defines
# the gtest and gtest_main targets.
add_subdirectory(${CMAKE_CURRENT_BINARY_DIR}/gtest/googletest-src
                 ${CMAKE_CURRENT_BINARY_DIR}/gtest/googletest-build
                 EXCLUDE_FROM_ALL)

# The gtest/gtest_main targets carry header search path
# dependencies automatically when using CMake 2.8.11 or
# later. Otherwise we have to add them here ourselves.
if (CMAKE_VERSION VERSION_LESS 2.8.11)
  include_directories("${gtest_SOURCE_DIR}/include")
endif()
# -- END SECTION -------------------------------------------------------------

include_directories (${gtest_SOURCE_DIR}/include)
file (GLOB TEST_FILES ${TEST_DIR}/*.cpp ${TEST_DIR}/*.h ${SRC_FILES}/*.h ${SRC_FILES}/HttpServer.cpp ${SRC_FILES}/HttpRequest.cpp)
add_executable(unit-test-runner test/main.cpp ${TEST_FILES})
target_link_libraries(unit-test-runner gtest_main)