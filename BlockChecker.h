/* (c) 2020 RNDr. Simon Toth (happy.cerberus@gmail.com) */
#ifndef SUDOKU_BLOCKCHECKER_H
#define SUDOKU_BLOCKCHECKER_H

#include <vector>
#include <cstdint>
#include "SudokuSquare.h"
#include "gtest/gtest_prod.h"

typedef std::vector<SquareType *> SudokuBlockType;

class BlockChecker {
public:
    BlockChecker(const SudokuBlockType &elements);

    bool Check() const;

    void Prune(unsigned number);

private:
    SudokuBlockType elem_;

    FRIEND_TEST(SudokuTest, SetupCheckers);
};


#endif //SUDOKU_BLOCKCHECKER_H
