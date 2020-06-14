/* (c) 2020 RNDr. Simon Toth (happy.cerberus@gmail.com) */
#ifndef SUDOKU_SUDOKU_H
#define SUDOKU_SUDOKU_H

#include "BlockChecker.h"
#include "Square.h"
#include <cstdint>
#include <iosfwd>
#include <unordered_set>
#include <vector>

enum SudokuTypes { BASIC = 1, DIAGONAL = 2 };

typedef std::vector<std::vector<sudoku::Square>> SudokuDataType;

namespace sudoku {
class Sudoku;

std::ostream &operator<<(std::ostream &s, const Sudoku &puzzle);
std::istream &operator>>(std::istream &s, Sudoku &puzzle);


class SudokuRow {
public:
  SudokuRow() = delete;
  SudokuRow(const SudokuRow &) = default;
  SudokuRow(SudokuRow &&) = default;

  Square &operator[](unsigned index) {
    assert(index < len_);
    return data_[index];
  }
  const Square &operator[](unsigned index) const {
    assert(index < len_);
    return data_[index];
  }

  unsigned Size() const { return len_; }

private:
  SudokuRow(Square *data, unsigned len) : data_(data), len_(len) {}

  Square *data_;
  unsigned len_;
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

  const Square &operator[](unsigned index) const {
    assert(index < len_);
    return data_[index];
  }

  unsigned Size() const { return len_; }

private:
  ConstSudokuRow(const Square *data, unsigned len) : data_(data), len_(len) {}

  const Square *data_;
  unsigned len_;
  friend class Sudoku;
};

class Sudoku {
public:
  /*! Construct an empty Sudoku of the given size and type.
   *
   * @param size Size of the Sudoku {9x9}, {16x16}, {25x25}.
   * @param type BASIC or DIAGONAL
   */
  Sudoku(unsigned size = 9, SudokuTypes type = BASIC);

  /*! Construct a pre-filled Sudoku fo the given type. Size inferred from data.
   *
   * @param data Data to pre-fill the puzzle with.
   * @param type BASIC or DIAGONAL.
   */
  Sudoku(SudokuDataType data, SudokuTypes type = BASIC);

  // Removed copy constructor.
  Sudoku(const Sudoku &) = delete;
  // Removed move constructor.
  Sudoku(Sudoku &&) = delete;
  // Removed assignment operator
  Sudoku &operator=(const Sudoku &) = delete;
  // Removed move assignment operator.
  Sudoku &operator=(Sudoku &&) = delete;

  /*! Square bracket operator to allow for 2D access.
   *
   * @param index Row index to return.
   * @return A wrapper object around a row in the puzzle.
   */
  SudokuRow operator[](unsigned index) {
    return SudokuRow(data_[index].data(), Size());
  }
  /*! Square bracket operator to allow for 2D access.
   *
   * @param index Row index to return.
   * @return A const wrapper object around a row in the puzzle.
   */
  ConstSudokuRow operator[](unsigned index) const {
    return ConstSudokuRow(data_[index].data(), Size());
  }

  /*! Return whether there is a changed square in the puzzle.
   *
   * @return True if a square was changed since last ResetChange(), false
   *         otherwise.
   */
  bool HasChange() const;
  //! Reset the changed flag on all squares in the puzzle.
  void ResetChange();
  /*! Return the set of blocks that contain changed squares.
   *
   * @return Set of blocks.
   */
  std::unordered_set<BlockChecker *> ChangedBlocks() const;

  //! Return the size of the Sudoku.
  unsigned Size() const { return size_; }

  //! Return a const reference to the list of blocks.
  const std::vector<BlockChecker> &Blocks() const { return checks_; }
  //! Return a reference to the list of blocks.
  std::vector<BlockChecker> &Blocks() { return checks_; }
  //! Return a const reference to the row blocks.
  const std::vector<BlockChecker *> &GetRowBlocks() const {
    return row_checks_;
  }
  //! Return a const reference to the column blocks.
  const std::vector<BlockChecker *> &GetColBlocks() const {
    return col_checks_;
  }

  //! Return first square that is not set.
  std::pair<unsigned, unsigned> FirstUnset() const;
  //! Return whether all squares are set.
  bool IsSet() const;
  //! Return whether there is any conflict in the puzzle.
  bool HasConflict(); // TODO: return what the conflict is.

  //! Debug print the puzzle (output the current possibilities for each square).
  void DebugPrint(std::ostream &s);

  //! Solve fish for a given size and a number.
  void SolveFish(unsigned size, unsigned number);

  //! Solve finned fish for a given size and a number.
  void SolveFinnedFish(unsigned size, unsigned number);

private:
  std::vector<BlockChecker> checks_;
  std::vector<BlockChecker *> row_checks_;
  std::vector<BlockChecker *> col_checks_;
  SudokuDataType data_;
  std::vector<std::vector<std::vector<BlockChecker *>>> block_mapping_;
  unsigned size_;
  std::string debug_;

  void SetupCheckers(unsigned size = 9, SudokuTypes type = BASIC);

  unsigned NumberOfSharedBlocks(std::pair<unsigned, unsigned> l,
                            std::pair<unsigned, unsigned> r) const;

  std::pair<unsigned,unsigned> GetFin(const std::vector<BlockChecker *>& blocks,
                  const std::vector<unsigned>& set,
                  unsigned number) const;

  friend std::vector<std::vector<std::vector<BlockChecker *>>> &
  TestGetMappings(Sudoku &s);

  friend std::istream &operator>>(std::istream &s, Sudoku &puzzle);
};
} // namespace sudoku

#endif // SUDOKU_SUDOKU_H
