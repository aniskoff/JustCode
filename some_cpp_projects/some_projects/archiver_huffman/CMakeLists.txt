cmake_minimum_required(VERSION 3.13)
project(2018_ha2_huffman_aniskoff)

set(CMAKE_CXX_STANDARD 17)

include_directories(.)

add_executable(2018_ha2_huffman_aniskoff
        huffman.cpp
        huffman.hpp
        main.cpp)
