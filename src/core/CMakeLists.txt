# (c) 2020 RNDr. Simon Toth (happy.cerberus@gmail.com)

SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

add_library(core BitSet.cpp BitSet.h UniqueBlock.cpp UniqueBlock.h GenericBlock.cpp GenericBlock.h)
add_library(sudoku_algorithms SudokuAlgorithms.cpp SudokuAlgorithms.h)