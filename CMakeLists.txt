cmake_minimum_required(VERSION 3.12)
project(dubins)

set(CMAKE_CXX_STANDARD 14)
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -Wall") 

set(PYBIND11_FINDPYTHON ON)

add_subdirectory(extern/pybind11)

include_directories(
    dubins/include
    ${pybind11_INCLUDE_DIRS}
)

pybind11_add_module(dubins
    dubins/src/dubins_pybind.cpp
    dubins/src/dubins.cpp
)