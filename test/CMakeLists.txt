cmake_minimum_required(VERSION 3.0)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "-Wall -Wextra -Wpedantic")

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O2")

set(CATCH_INCLUDE_DIR "." CACHE PATH "Catch library include directory")

# List with test names, should be same as *.cpp files, but without extensions
set(TEST_NAMES "cmake_test")

include_directories(${CATCH_INCLUDE_DIR})

foreach(TEST ${TEST_NAMES})
  add_executable(${TEST} "${TEST}.cpp")
endforeach()