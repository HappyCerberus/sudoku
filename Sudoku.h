/* (c) 2020 RNDr. Simon Toth (happy.cerberus@gmail.com) */
#ifndef SUDOKU_SUDOKU_H
#define SUDOKU_SUDOKU_H

#include "BlockChecker.h"
#include "gtest/gtest_prod.h"
#include <iosfwd>
#include <unordered_set>
#include <vector>
#include <cstdint>

enum SudokuTypes {
    BASIC = 1,
    DIAGONAL = 2
};

typedef std::vector<std::vector<SquareType>> SudokuDataType;

class Sudoku {
public:
    Sudoku(unsigned size = 9, SudokuTypes type = BASIC);

    Sudoku(SudokuDataType data, SudokuTypes type = BASIC);

    Sudoku(const Sudoku &) = delete;

    Sudoku(Sudoku &&) = delete;

    Sudoku &operator=(const Sudoku &) = delete;

    Sudoku &operator=(Sudoku &&) = delete;

    bool CheckPuzzle();

    SudokuDataType &data() { return data_; }

    const SudokuDataType &data() const { return data_; }

    void Prune(size_t x, size_t y);

    void Prune(size_t x, size_t y, unsigned number);

    void DebugPrint(std::ostream &s);


private:
    std::vector<BlockChecker> checks_;
    SudokuDataType data_;
    std::vector<std::vector<std::vector<BlockChecker *>>> block_mapping_;

    void SetupCheckers(unsigned size = 9, SudokuTypes type = BASIC);

    FRIEND_TEST(SudokuTest, SetupCheckers);
};

std::ostream &operator<<(std::ostream &s, const Sudoku &puzzle);

std::istream &operator>>(std::istream &s, Sudoku &puzzle);

#endif //SUDOKU_SUDOKU_H
