/* (c) 2020 RNDr. Simon Toth (happy.cerberus@gmail.com) */
#ifndef SUDOKU_SUDOKU_H
#define SUDOKU_SUDOKU_H

#include "BlockChecker.h"
#include "Square.h"
#include "gtest/gtest_prod.h"
#include <cstdint>
#include <iosfwd>
#include <unordered_set>
#include <vector>

enum SudokuTypes { BASIC = 1, DIAGONAL = 2 };

typedef std::vector<std::vector<sudoku::Square>> SudokuDataType;

namespace sudoku {
class SudokuRow {
public:
  SudokuRow() = delete;
  SudokuRow(const SudokuRow &) = default;
  SudokuRow(SudokuRow &&) = default;

  Square &operator[](size_t index) {
    assert(index < len_);
    return data_[index];
  }
  const Square &operator[](size_t index) const {
    assert(index < len_);
    return data_[index];
  }

private:
  SudokuRow(Square *data, size_t len) : data_(data), len_(len) {}

  Square *data_;
  size_t len_;
  friend class Sudoku;
  friend class ConstSudokuRow;
};

class ConstSudokuRow {
public:
  ConstSudokuRow() = delete;
  ConstSudokuRow(const ConstSudokuRow &) = default;
  ConstSudokuRow(ConstSudokuRow &&) = default;
  ConstSudokuRow(const SudokuRow &r) : data_(r.data_), len_(r.len_) {}
  ConstSudokuRow(SudokuRow &&r) : data_(r.data_), len_(r.len_) {}

  const Square &operator[](size_t index) const {
    assert(index < len_);
    return data_[index];
  }

private:
  ConstSudokuRow(const Square *data, size_t len) : data_(data), len_(len) {}

  const Square *data_;
  size_t len_;
  friend class Sudoku;
};

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

  SudokuRow operator[](size_t index) {
    return SudokuRow(data_[index].data(), data_[index].size());
  }
  ConstSudokuRow operator[](size_t index) const {
      return ConstSudokuRow(data_[index].data(), data_[index].size());
  }

  unsigned Size() { return static_cast<unsigned>(data_.size()); }

  SudokuDataType &data() {
    return data_;
  }

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
}

#endif // SUDOKU_SUDOKU_H
