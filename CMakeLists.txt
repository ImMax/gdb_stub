cmake_minimum_required(VERSION 3.16)


project(gdb_stub)

set(CMAKE_CXX_STANDARD 11)

file(GLOB_RECURSE HEADER_FILES ./include/*.hpp)
file(GLOB_RECURSE SOURCE_FILES ./src/*.cpp)

include_directories(include)

add_subdirectory(test)

add_library(gdb_stub STATIC ${HEADER_FILES} ${SOURCE_FILES})
