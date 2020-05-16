# (c) 2020 RNDr. Simon Toth (happy.cerberus@gmail.com)
cmake_minimum_required(VERSION 3.10)
project(sudoku)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "-O3 -Wall -Wextra")
add_executable(sudoku Sudoku.cpp Sudoku.h SudokuSquare.h SudokuSquare.cpp BlockChecker.cpp BlockChecker.h main.cpp)