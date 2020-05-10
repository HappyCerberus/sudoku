/* (c) 2020 RNDr. Simon Toth (happy.cerberus@gmail.com) */

#include <iostream>

// Ultimate goal:
// Sudoku generator, that understands how humans solve Sudoku.
// - can provide difficulty rating
// - can generate from a template
// - support all the variants of Sudoku

// [DONE] Basic Solver - backtracking


#include <vector>

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
            if (sudoku.data()[i][j] == 0)
                return std::make_pair(i, j);
        }
    }
    return std::make_pair(-1, -1);
}

bool puzzle_filled(const Sudoku &sudoku) {
    for (int i = 0; i < sudoku.data().size(); i++) {
        for (int j = 0; j < sudoku.data()[i].size(); j++) {
            if (sudoku.data()[i][j] == 0)
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
    for (int k = 1; k <= 9; k++) {
        sudoku.data()[next.first][next.second] = k;
        if (solve_recursive(sudoku))
            return true;
    }
    sudoku.data()[next.first][next.second] = 0;
    return false;
}

// Version 1 is done
// We can solve sudokus & diagonal sudokus and maybe 16x16 sudokus?
// Next steps:
// - stack based non-recursive solution
// - write tests
// - fix the access to the elements of the puzzle Get(int x, int y); operator[]
// - lookup data format for sudokus
// - encapsulate the solver a class
// - design a new puzzle data representation that is suitable for human solving


int main() {
    auto data = std::vector<std::vector<int>>{
            std::vector<int>{0, 6, 0, 8, 0, 0, 0, 0, 9},
            std::vector<int>{0, 1, 0, 0, 9, 0, 7, 0, 0},
            std::vector<int>{0, 3, 2, 0, 0, 0, 0, 6, 0},
            std::vector<int>{0, 0, 0, 0, 7, 6, 1, 0, 4},
            std::vector<int>{0, 0, 9, 2, 0, 3, 6, 0, 0},
            std::vector<int>{6, 0, 7, 4, 1, 0, 0, 0, 0},
            std::vector<int>{0, 9, 0, 0, 0, 0, 2, 3, 0},
            std::vector<int>{0, 0, 6, 0, 8, 0, 0, 4, 0},
            std::vector<int>{5, 0, 0, 0, 0, 4, 0, 7, 0},
    };

    auto data2 = std::vector<std::vector<int>>{
            std::vector<int>{0, 0, 5, 0, 7, 0, 3, 0, 0},
            std::vector<int>{8, 0, 0, 0, 5, 0, 0, 0, 4},
            std::vector<int>{0, 9, 0, 6, 0, 4, 0, 8, 0},
            std::vector<int>{0, 0, 9, 0, 0, 0, 6, 0, 0},
            std::vector<int>{3, 0, 0, 0, 0, 0, 0, 0, 8},
            std::vector<int>{0, 0, 1, 0, 0, 0, 9, 0, 0},
            std::vector<int>{0, 7, 0, 4, 0, 2, 0, 3, 0},
            std::vector<int>{6, 0, 0, 0, 9, 0, 0, 0, 2},
            std::vector<int>{0, 0, 2, 0, 8, 0, 4, 0, 0},
    };

    auto data3 = std::vector<std::vector<int>>{
            std::vector<int>{0, 0, 16, 0, 0, 0, 0, 0, 15, 14, 0, 2, 0, 6, 0, 13},
            std::vector<int>{0, 0, 0, 0, 3, 2, 0, 0, 5, 0, 0, 0, 0, 0, 10, 14},
            std::vector<int>{0, 8, 0, 0, 0, 0, 0, 0, 0, 0, 16, 6, 0, 12, 3, 0},
            std::vector<int>{10, 0, 0, 0, 0, 0, 0, 14, 12, 7, 3, 0, 5, 0, 1, 0},
            std::vector<int>{0, 0, 0, 5, 6, 15, 2, 0, 3, 16, 0, 8, 0, 9, 0, 0},
            std::vector<int>{12, 7, 3, 8, 10, 0, 1, 16, 0, 0, 0, 0, 2, 11, 0, 0},
            std::vector<int>{0, 9, 0, 0, 0, 0, 13, 11, 2, 0, 0, 15, 0, 0, 0, 10},
            std::vector<int>{0, 6, 0, 0, 0, 5, 0, 0, 0, 0, 0, 1, 0, 4, 14, 0},
            std::vector<int>{0, 0, 0, 2, 0, 0, 0, 3, 14, 0, 0, 0, 7, 0, 0, 0},
            std::vector<int>{0, 0, 0, 3, 0, 16, 0, 8, 0, 1, 6, 0, 0, 0, 15, 0},
            std::vector<int>{6, 0, 0, 0, 14, 0, 9, 0, 0, 0, 4, 0, 0, 16, 0, 0},
            std::vector<int>{0, 0, 8, 0, 0, 7, 0, 10, 0, 2, 0, 0, 0, 14, 11, 1},
            std::vector<int>{8, 0, 0, 0, 13, 10, 7, 0, 0, 0, 0, 0, 16, 0, 0, 0},
            std::vector<int>{0, 0, 0, 0, 4, 0, 0, 1, 7, 10, 13, 0, 9, 0, 8, 11},
            std::vector<int>{9, 0, 1, 0, 2, 0, 12, 0, 0, 5, 0, 0, 0, 0, 4, 0},
            std::vector<int>{0, 0, 14, 0, 0, 0, 11, 15, 0, 0, 0, 4, 10, 0, 0, 5},
    };

    Sudoku test{data};
    Sudoku test2{data2, DIAGONAL};
    Sudoku test3(data3);

    if (!solve_recursive(test)) {
        std::cout << "There is something very wrong!" << std::endl;
    }
    if (!solve_recursive(test2)) {
        std::cout << "There is something very wrong!" << std::endl;
    }
    if (!solve_recursive(test3)) {
        std::cout << "There is something very wrong!" << std::endl;
    }

    std::cout << test << std::endl;
    std::cout << "-----------------------------" << std::endl;
    std::cout << test2 << std::endl;
    std::cout << "-----------------------------" << std::endl;
    std::cout << test3 << std::endl;
    return 0;
}
