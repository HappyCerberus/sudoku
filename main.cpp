/* (c) 2020 RNDr. Simon Toth (happy.cerberus@gmail.com) */

#include <iostream>

// Ultimate goal:
// Sudoku generator, that understands how humans solve Sudoku.
// - can provide difficulty rating
// - can generate from a template
// - support all the variants of Sudoku

// [DONE] Basic Solver - backtracking


#include <vector>
#include <sstream>
#include "Sudoku.h"

// Basic Sudoku 9x9
// - rows
// - columns
// - blocks 3x3

// Diagonal
// - basic sudoku
// - 2 diagonals

std::pair<int, int> find_empty(const Sudoku &sudoku) {
    for (int i = 0; i < sudoku.data().size(); i++) {
        for (int j = 0; j < sudoku.data()[i].size(); j++) {
            if (!sudoku.data()[i][j].IsSet())
                return std::make_pair(i, j);
        }
    }
    return std::make_pair(-1, -1);
}

bool puzzle_filled(const Sudoku &sudoku) {
    for (int i = 0; i < sudoku.data().size(); i++) {
        for (int j = 0; j < sudoku.data()[i].size(); j++) {
            if (!sudoku.data()[i][j].IsSet())
                return false;
        }
    }

    return true;
}

bool solve_recursive(Sudoku &sudoku) {
    if (!sudoku.CheckPuzzle()) {
        return false;
    }
    if (puzzle_filled(sudoku) && sudoku.CheckPuzzle()) {
        return true;
    }

    auto next = find_empty(sudoku);
    SquareType original = sudoku.data()[next.first][next.second];
    unsigned v = 0;
    while ((v = original.Next(v)) != 0) {
        sudoku.data()[next.first][next.second].Set(v);
        if (solve_recursive(sudoku))
            return true;
    }
    sudoku.data()[next.first][next.second] = original;
    return false;
}

// Version 2 is done
// We can solve sudokus & diagonal sudokus and maybe 16x16 sudokus?
// Next steps:
// - test performance V1 vs V2 using a profiler
// - stack based non-recursive solution
// - write tests
// - fix the access to the elements of the puzzle Get(int x, int y); operator[]
// - lookup data format for sudokus
// - encapsulate the solver a class
// - design a new puzzle data representation that is suitable for human solving


int main() {

    std::string small = "4  0  0   0  0  8   0  0  3 \n"
                        "0  0  5   2  0  0   0  1  0 \n"
                        "0  6  0   0  0  9   0  0  0 \n"
                        "\n"
                        "0  0  0   0  0  0   0  3  0 \n"
                        "0  0  6   9  0  1   0  0  0 \n"
                        "0  0  0   6  0  4   9  2  0 \n"
                        "\n"
                        "0  2  9   0  0  0   3  0  0 \n"
                        "0  0  4   0  0  2   0  8  5 \n"
                        "0  0  0   7  0  3   0  0  0 ";

    std::string sudoku =
            "*  9  *  *   *  *  C  *   *  *  *  *   *  *  *  *"
            "*  D  *  *   *  E  6  *   1  *  *  9   *  4  *  A"
            "*  *  F  5   *  *  *  *   8  6  *  *   1  7  C  9"
            "*  *  C  *   *  *  *  4   3  0  5  7   D  *  B  E"
            "*  *  *  *   *  *  *  6   *  1  *  A   *  *  *  F"
            "*  *  *  *   *  *  9  *   C  *  *  6   *  *  A  *"
            "*  6  *  *   8  *  F  *   *  *  D  *   *  *  *  *"
            "*  4  *  *   D  5  *  1   *  *  B  *   *  C  *  *"
            "*  0  *  *   3  C  *  *   7  *  9  5   B  A  *  *"
            "D  5  *  4   *  *  *  9   0  *  *  F   *  *  *  6"
            "8  *  *  7   *  *  E  *   *  *  *  *   5  *  *  2"
            "*  F  *  2   *  *  *  7   *  *  3  *   C  *  D  *"
            "*  *  B  F   *  *  1  A   5  *  4  *   0  *  *  C"
            "*  1  *  E   *  0  5  *   *  B  7  *   *  *  9  8"
            "C  7  *  6   E  *  8  *   *  *  *  *   *  *  *  3"
            "*  *  5  A   *  *  *  B   *  E  *  8   6  *  2  *";

    std::stringstream stream(small);
    Sudoku test3(9);
    stream >> test3;

    // go over all set values
    // get all blocks the value is in
    // prune all squares in that block
/*
    for (size_t i = 0; i < test3.data().size(); i++) {
        for (size_t j = 0; j < test3.data()[i].size(); j++) {
            if (!test3.data()[i][j].IsSet())
                continue;
            test3.Prune(i,j);
        }
    }
*/
    std::cout << test3 << std::endl;

    if (!solve_recursive(test3)) {
        std::cout << "There is something very wrong!" << std::endl;
    }

    std::cout << test3 << std::endl;
    return 0;
}
