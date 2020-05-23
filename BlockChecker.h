/* (c) 2020 RNDr. Simon Toth (happy.cerberus@gmail.com) */
#ifndef SUDOKU_BLOCKCHECKER_H
#define SUDOKU_BLOCKCHECKER_H

#include "SudokuSquare.h"
#include "gtest/gtest_prod.h"
#include <cstdint>
#include <vector>

typedef std::vector<SquareType *> SudokuBlockType;

class BlockChecker {
public:
  BlockChecker(const SudokuBlockType &elements);

  bool Check() const;

  void Prune(unsigned number);

  void Solve();

private:
  SudokuBlockType elem_;

  FRIEND_TEST(SudokuTest, SetupCheckers);
};

void recursive_set_find(std::vector<std::vector<size_t>> &result,
                        const std::vector<SquareType *> &squares, size_t size,
                        size_t consumed = 0,
                        std::vector<size_t> path = std::vector<size_t>{},
                        size_t first_index = 0);

void recursive_number_find(std::vector<std::vector<size_t>> &result,
                           const std::vector<SquareType *> &squares,
                           size_t size, size_t consumed = 0,
                           std::vector<size_t> numbers = std::vector<size_t>{},
                           size_t first_index = 0);

#endif // SUDOKU_BLOCKCHECKER_H
