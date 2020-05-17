/* (c) 2020 RNDr. Simon Toth (happy.cerberus@gmail.com) */

#include "Sudoku.h"
#include <ostream>
#include <unordered_map>
#include <iomanip>

Sudoku::Sudoku(unsigned size, SudokuTypes type) : data_(size, std::vector<SquareType>(size, SquareType{size})),
                                                  block_mapping_(size,
                                                                 std::vector<std::vector<BlockChecker *>>(size,
                                                                                                          std::vector<BlockChecker *>())) {
    SetupCheckers(size, type);
}

Sudoku::Sudoku(SudokuDataType data, SudokuTypes type) : data_(std::move(data)), block_mapping_(data_.size(),
                                                                                               std::vector<std::vector<BlockChecker *>>(
                                                                                                       data_.size(),
                                                                                                       std::vector<BlockChecker *>())) {
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
    switch (type) {
        case BASIC:
            checks_.reserve(3 * size);
            break;
        case DIAGONAL:
            checks_.reserve(3 * size + 2);
    }

    for (size_t i = 0; i < size; i++) {
        SudokuBlockType row;
        for (size_t j = 0; j < size; j++) {
            row.push_back(&data_[i][j]);
        }
        checks_.emplace_back(row);
        for (size_t j = 0; j < size; j++) {
            block_mapping_[i][j].push_back(&checks_[checks_.size() - 1]);
        }
    }

    for (size_t j = 0; j < size; j++) {
        SudokuBlockType column;
        for (size_t i = 0; i < size; i++) {
            column.push_back(&data_[i][j]);
        }
        checks_.emplace_back(column);
        for (size_t i = 0; i < size; i++) {
            block_mapping_[i][j].push_back(&checks_[checks_.size() - 1]);
        }
    }

    std::unordered_map<unsigned, unsigned> blocksizes = {
            {9,  3},
            {16, 4}
    };
    unsigned bsize = blocksizes[size];
    for (size_t i = 0; i < bsize; i++) {
        for (size_t j = 0; j < bsize; j++) {
            SudokuBlockType block;
            for (size_t x = i * bsize; x < (i + 1) * bsize; x++) {
                for (size_t y = j * bsize; y < (j + 1) * bsize; y++) {
                    block.push_back(&data_[x][y]);
                }
            }
            checks_.emplace_back(block);
            for (size_t x = i * bsize; x < (i + 1) * bsize; x++) {
                for (size_t y = j * bsize; y < (j + 1) * bsize; y++) {
                    block_mapping_[x][y].push_back(&checks_[checks_.size() - 1]);
                }
            }
        }
    }

    if (type == BASIC)
        return;

    SudokuBlockType d1, d2;
    for (size_t i = 0; i < size; i++) {
        d1.push_back(&data_[i][i]);
        d2.push_back(&data_[i][size - 1 - i]);
    }
    checks_.emplace_back(d1);
    checks_.emplace_back(d2);
    for (size_t i = 0; i < size; i++) {
        block_mapping_[i][i].push_back(&checks_[checks_.size() - 2]);
        block_mapping_[i][size - 1 - i].push_back(&checks_[checks_.size() - 1]);
    }
}

void Sudoku::Prune(size_t x, size_t y, unsigned int number) {
    for (auto *b : block_mapping_[x][y]) {
        b->Prune(number);
    }
}

void Sudoku::Prune(size_t x, size_t y) {
    Prune(x, y, data_[x][y].Value());
}

void Sudoku::DebugPrint(std::ostream &s) {
    for (size_t i = 0; i < data_.size(); i++) {
        for (size_t j = 0; j < data_[i].size(); j++) {
            data_[i][j].DebugPrint(s);
            s << " ";
        }
        s << std::endl;
    }
}


std::ostream &operator<<(std::ostream &s, const Sudoku &puzzle) {
    for (size_t i = 0; i < puzzle.data().size(); i++) {
        for (size_t j = 0; j < puzzle.data()[i].size(); j++) {
            if (puzzle.data()[i][j].Value() >= 10) {
                s << std::setw(2) << static_cast<char>(puzzle.data()[i][j].Value() - 10 + 'A');
            } else {
                s << std::setw(2) << puzzle.data()[i][j].Value();
            }
        }
        s << std::endl;
    }
    return s;
}
//https://stackoverflow.com/questions/799599/c-custom-stream-manipulator-that-changes-next-item-on-stream

/*
*  9  *  *   *  *  C  *   *  *  *  *   *  *  *  *
*  D  *  *   *  E  6  *   1  *  *  9   *  4  *  A
*  *  F  5   *  *  *  *   8  6  *  *   1  7  C  9
*  *  C  *   *  *  *  4   3  0  5  7   D  *  B  E

*  *  *  *   *  *  *  6   *  1  *  A   *  *  *  F
*  *  *  *   *  *  9  *   C  *  *  6   *  *  A  *
*  6  *  *   8  *  F  *   *  *  D  *   *  *  *  *
*  4  *  *   D  5  *  1   *  *  B  *   *  C  *  *

*  0  *  *   3  C  *  *   7  *  9  5   B  A  *  *
D  5  *  4   *  *  *  9   0  *  *  F   *  *  *  6
8  *  *  7   *  *  E  *   *  *  *  *   5  *  *  2
*  F  *  2   *  *  *  7   *  *  3  *   C  *  D  *

*  *  B  F   *  *  1  A   5  *  4  *   0  *  *  C
*  1  *  E   *  0  5  *   *  B  7  *   *  *  9  8
C  7  *  6   E  *  8  *   *  *  *  *   *  *  *  3
*  *  5  A   *  *  *  B   *  E  *  8   6  *  2  *
*/
std::istream &operator>>(std::istream &s, Sudoku &puzzle) {
    for (size_t i = 0; i < puzzle.data().size(); i++) {
        for (size_t j = 0; j < puzzle.data()[i].size(); j++) {
            char c;
            s >> c;
            if (c == '*' || c == '0') {
                puzzle.data()[i][j].Reset();
                continue;
            }
            if (isalpha(c)) puzzle.data()[i][j].Set(c - 'A' + 10);
            if (isdigit(c)) puzzle.data()[i][j].Set(c - '0');

        }
    }
    return s;
}