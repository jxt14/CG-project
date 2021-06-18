cmake_minimum_required(VERSION 3.10)
project(Lighthouse)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_FLAGS "-O2")

add_executable(lighthouse main.cpp)
add_executable(generation_test random_polygon_test.cpp)
add_executable(histogram_test histo_part_test.cpp)
add_executable(solve_test solve_test.cpp)



