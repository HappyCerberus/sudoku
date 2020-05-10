/* (c) 2020 RNDr. Simon Toth (happy.cerberus@gmail.com) */

#include "Sudoku.h"
#include <ostream>
#include <unordered_map>
#include <iomanip>

Sudoku::Sudoku(unsigned size, SudokuTypes type) : data_(size, std::vector<int>(size, 0)) {
    SetupCheckers(size, type);
}

Sudoku::Sudoku(std::vector<std::vector<int>> data, SudokuTypes type) : data_(std::move(data)) {
    SetupCheckers(data_.size(), type);
}

bool Sudoku::CheckPuzzle() {
    for (const auto &check : checks_) {
        if (!check.Check())
            return false;
    }
    return true;
}

void Sudoku::SetupCheckers(unsigned int size, SudokuTypes type) {
    for (int i = 0; i < size; i++) {
        std::vector<int *> row;
        for (int j = 0; j < size; j++) {
            row.push_back(&data_[i][j]);
        }
        checks_.emplace_back(row);
    }

    for (int j = 0; j < size; j++) {
        std::vector<int *> column;
        for (int i = 0; i < size; i++) {
            column.push_back(&data_[i][j]);
        }
        checks_.emplace_back(column);
    }

    std::unordered_map<int, int> blocksizes = {
            {9,  3},
            {16, 4}
    };
    int bsize = blocksizes[size];
    for (int i = 0; i < bsize; i++) {
        for (int j = 0; j < bsize; j++) {
            std::vector<int *> block;
            for (int x = i * bsize; x < (i + 1) * bsize; x++) {
                for (int y = j * bsize; y < (j + 1) * bsize; y++) {
                    block.push_back(&data_[x][y]);
                }
            }
            checks_.emplace_back(block);
        }
    }

    if (type == BASIC)
        return;

    std::vector<int *> d1, d2;
    for (int i = 0; i < size; i++) {
        d1.push_back(&data_[i][i]);
        d2.push_back(&data_[i][size - 1 - i]);
    }
    checks_.emplace_back(d1);
    checks_.emplace_back(d2);
}

std::ostream &operator<<(std::ostream &s, const Sudoku &puzzle) {
    int width = 2;
    if (puzzle.data().size() > 9) {
        width++;
    }
    for (int i = 0; i < puzzle.data().size(); i++) {
        for (int j = 0; j < puzzle.data()[i].size(); j++) {
            s << std::setw(width) << puzzle.data()[i][j];
        }
        s << std::endl;
    }
    return s;
}
