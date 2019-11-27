cmake_minimum_required(VERSION 3.0)

if(POLICY CMP0048)
  cmake_policy(SET CMP0048 NEW) # CMake 3.0.0
endif()

if(POLICY CMP0015)
  cmake_policy(SET CMP0015 NEW)
endif(POLICY CMP0015)

# Needed for CMake 2.12+
if(POLICY CMP0077)
  cmake_policy(SET CMP0077 NEW)
endif()

project(levelctl CXX)

set(CMAKE_CXX_STANDARD 17)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -s")

# Global Options
option(BUILD_SHARED OFF)
option(BUILD_TESTING OFF)
set(BUILD_SHARED_LIBRARIES OFF)

# Leveldb Options
option(LEVELDB_BUILD_TESTS OFF)
option(LEVELDB_BUILD_BENCHMARKS OFF)
option(LEVELDB_INSTALL OFF)

# Snappy Options
option(SNAPPY_BUILD_TESTS OFF)
option(SNAPPY_INSTALL OFF)

include_directories(lib/args)
include_directories(lib/leveldb/include)

add_subdirectory(lib/leveldb)
add_subdirectory(lib/snappy)

file(GLOB SRCS src/*.cpp)
set(LIBS leveldb snappy)

add_executable(levelctl ${SRCS})
target_link_libraries(levelctl ${LIBS})