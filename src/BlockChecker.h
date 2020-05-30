/* (c) 2020 RNDr. Simon Toth (happy.cerberus@gmail.com) */
#ifndef SUDOKU_BLOCKCHECKER_H
#define SUDOKU_BLOCKCHECKER_H

#include "Square.h"
#include "gtest/gtest_prod.h"
#include <cstdint>
#include <unordered_set>
#include <vector>

typedef std::vector<::sudoku::SquareType *> SudokuBlockType;

class BlockChecker {
public:
  BlockChecker(const SudokuBlockType &elements);

  bool Check() const;

  void Prune(unsigned number);
  void Prune(unsigned number, const std::vector<size_t> &whitelist);

  void Solve();

  std::unordered_set<size_t> NumberPositions(size_t number) const;
  void NumberPositions(size_t number,
                       std::unordered_set<size_t> &positions) const;

  void PruneInterection(BlockChecker &r) const;
  const std::vector<sudoku::SquareType *> &GetSquares() const { return elem_; }

private:
  SudokuBlockType elem_;

  FRIEND_TEST(SudokuTest, SetupCheckers);
};

void recursive_set_find(std::vector<std::vector<size_t>> &result,
                        const std::vector<sudoku::SquareType *> &squares, size_t size);

void recursive_number_find(std::vector<std::vector<size_t>> &result,
                           const std::vector<sudoku::SquareType *> &squares,
                           size_t size);

// TODO: write tests
void recursive_swordfish_find(std::vector<std::vector<size_t>> &result,
                              const std::vector<BlockChecker *> &blocks,
                              size_t size, size_t number);

#endif // SUDOKU_BLOCKCHECKER_H
