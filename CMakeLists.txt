# (c) 2020 RNDr. Simon Toth (happy.cerberus@gmail.com)
cmake_minimum_required(VERSION 3.10)
project(sudoku)

set(CMAKE_CXX_STANDARD 17)

add_executable(sudoku Sudoku.cpp Sudoku.h BlockChecker.cpp BlockChecker.h main.cpp)