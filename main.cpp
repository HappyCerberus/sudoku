/* (c) 2020 RNDr. Simon Toth (happy.cerberus@gmail.com) */

#include <iostream>

// Ultimate goal:
// Sudoku generator, that understands how humans solve Sudoku.
// - can provide difficulty rating
// - can generate from a template
// - support all the variants of Sudoku

// Basic Solver:
// - backtracking solver
// - make a guess, check constrains
//   - if its wrong - go back, remove the guess
//   - if its correct, we just continue

#include <vector>

struct Sudoku {
    Sudoku() : data(9, std::vector<int>(9, 0)) {}

    Sudoku(int a1, int a2, int b1, int b2) {
        data.resize(2);
        data[0].resize(2);
        data[1].resize(2);
        data[0][0] = a1;
        data[0][1] = a2;
        data[1][0] = b1;
        data[1][1] = b2;
    }

    std::vector<std::vector<int>> data;
};

// Basic sudoku
// rows - all_distinct
// columns - all_distinct
// squares 3x3 - all_distinct
bool check_puzzle(const Sudoku &sudoku) {
    // check rows
    for (int i = 0; i < sudoku.data.size(); i++) {
        std::vector<bool> present(sudoku.data[i].size(), false);
        for (int j = 0; j < sudoku.data[i].size(); j++) {
            if (sudoku.data[i][j] == 0)
                continue;
            if (present[sudoku.data[i][j] - 1]) {
                return false;
            }
            present[sudoku.data[i][j] - 1] = true;
        }
    }
    // check columns
    for (int j = 0; j < sudoku.data[0].size(); j++) {
        std::vector<bool> present(sudoku.data[0].size(), false);
        for (int i = 0; i < sudoku.data.size(); i++) {
            if (sudoku.data[i][j] == 0)
                continue;
            if (present[sudoku.data[i][j] - 1]) {
                return false;
            }
            present[sudoku.data[i][j] - 1] = true;
        }
    }
    // check blocks
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            std::vector<bool> present(sudoku.data[0].size(), false);
            for (int x = i * 3; x < (i + 1) * 3; x++) {
                for (int y = j * 3; y < (j + 1) * 3; y++) {
                    if (sudoku.data[x][y] == 0)
                        continue;
                    if (present[sudoku.data[x][y] - 1]) {
                        return false;
                    }
                    present[sudoku.data[x][y] - 1] = true;
                }
            }
        }
    }
    return true;
}

std::pair<int, int> find_empty(const Sudoku &sudoku) {
    for (int i = 0; i < sudoku.data.size(); i++) {
        for (int j = 0; j < sudoku.data[i].size(); j++) {
            if (sudoku.data[i][j] == 0)
                return std::make_pair(i, j);
        }
    }
    return std::make_pair(-1, -1);
}

bool puzzle_filled(const Sudoku &sudoku) {
    for (int i = 0; i < sudoku.data.size(); i++) {
        for (int j = 0; j < sudoku.data[i].size(); j++) {
            if (sudoku.data[i][j] == 0)
                return false;
        }
    }

    return true;
}

bool solve_recursive(Sudoku &sudoku) {
    if (!check_puzzle(sudoku)) {
        return false;
    }
    if (puzzle_filled(sudoku) && check_puzzle(sudoku)) {
        return true;
    }

    auto next = find_empty(sudoku);
    for (int k = 1; k <= 9; k++) {
        sudoku.data[next.first][next.second] = k;
        if (solve_recursive(sudoku))
            return true;
    }
    sudoku.data[next.first][next.second] = 0;
    return false;
}

// Version 0 is done
// We can solve sudokus.
// Everything is very much hardcoded:
// - un-hard-code things
// - stack based non-recursive solution
// - write tests
// - ostream operator
// - we need a cleanup of the check routine
//  - iterator over a block
//  - we could have objects that check the state of a block
//    - for each block we would have one object
//    - they point at the correct elements in the arrays


int main() {
    Sudoku test{};
    test.data = std::vector<std::vector<int>>{
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


    //Sudoku sudoku{};
    if (!solve_recursive(test)) {
        std::cout << "There is something very wrong!" << std::endl;
    }

    for (int i = 0; i < test.data.size(); i++) {
        for (int j = 0; j < test.data[i].size(); j++) {
            std::cout << test.data[i][j] << " ";
        }
        std::cout << std::endl;
    }

    return 0;
}
