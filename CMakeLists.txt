cmake_minimum_required(VERSION 3.15)
project(differentiator)

set(CMAKE_CXX_STANDARD 17)

add_executable(differentiator main.cpp tree.h fileRead.h dsl.h)