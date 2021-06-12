cmake_minimum_required(VERSION 3.10)
project(Lighthouse)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_FLAGS "-O2")

# aux_source_directory(. SRC_LIST)
add_executable(lighthouse main.cpp)
# add_executable(rand_polygon rand.cpp)


