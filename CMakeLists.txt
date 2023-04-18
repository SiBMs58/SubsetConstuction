cmake_minimum_required(VERSION 3.24)
project(SubsetConstruction)

set(CMAKE_CXX_STANDARD 17)

add_executable(SubsetConstruction main.cpp NFA.cpp DFA.cpp input-ssc-cursus.json input-ssc1.json input-ssc2.json DFA.json)
