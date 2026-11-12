cmake_minimum_required(VERSION 3.20)
project(Gamma)

set(CMAKE_CXX_STANDARD 20)

add_executable(Gamma src/main.cpp
        src/arena.hpp
        src/generation.hpp
        src/parser.hpp
        src/tokenization.hpp)
