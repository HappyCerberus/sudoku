/* (c) 2020 RNDr. Simon Toth (happy.cerberus@gmail.com) */
#ifndef SUDOKU_SUDOKU_H
#define SUDOKU_SUDOKU_H

#include "BlockChecker.h"
#include "gtest/gtest_prod.h"
#include <cstdint>
#include <iosfwd>
#include <unordered_set>
#include <vector>

enum SudokuTypes { BASIC = 1, DIAGONAL = 2 };

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

  bool HasChange() const;
  void ResetChange();
  std::unordered_set<BlockChecker *> ChangedBlocks() const;
  const std::vector<BlockChecker> &Blocks() const { return checks_; }
  std::vector<BlockChecker> &Blocks() { return checks_; }

  SudokuDataType &data() { return data_; }

  const SudokuDataType &data() const { return data_; }

  void Prune(size_t x, size_t y);

  void Prune(size_t x, size_t y, unsigned number);

  void DebugPrint(std::ostream &s);

  const std::vector<BlockChecker *> &GetRowBlocks() const {
    return row_checks_;
  }
  const std::vector<BlockChecker *> &GetColBlocks() const {
    return col_checks_;
  }

  size_t Size() { return size_; }

private:
  std::vector<BlockChecker> checks_;
  std::vector<BlockChecker *> row_checks_;
  std::vector<BlockChecker *> col_checks_;
  SudokuDataType data_;
  std::vector<std::vector<std::vector<BlockChecker *>>> block_mapping_;
  size_t size_;

  void SetupCheckers(unsigned size = 9, SudokuTypes type = BASIC);

  FRIEND_TEST(SudokuTest, SetupCheckers);
};

std::ostream &operator<<(std::ostream &s, const Sudoku &puzzle);

std::istream &operator>>(std::istream &s, Sudoku &puzzle);

#endif // SUDOKU_SUDOKU_H
