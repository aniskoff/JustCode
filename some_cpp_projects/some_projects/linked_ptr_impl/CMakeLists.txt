cmake_minimum_required(VERSION 3.13)
project(linked_ptr)

set(CMAKE_CXX_STANDARD 17)

add_custom_target(linked_ptr COMMAND make
        CLION_EXE_DIR=${PROJECT_BINARY_DIR})