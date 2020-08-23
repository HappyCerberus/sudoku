/* (c) 2020 RNDr. Simon Toth (happy.cerberus@gmail.com) */
#ifndef SUDOKU_SUDOKU_H
#define SUDOKU_SUDOKU_H

#include "core/BitSet.h"
#include "core/UniqueBlock.h"
#include <cstdint>
#include <cstring>
#include <functional>
#include <iosfwd>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <string>
#include <vector>

enum SudokuTypes { BASIC = 1, DIAGONAL = 2 };

namespace sudoku {
class Sudoku;

std::ostream &operator<<(std::ostream &s, const Sudoku &puzzle);
std::istream &operator>>(std::istream &s, Sudoku &puzzle);

class SudokuRow {
public:
  SudokuRow() = delete;
  SudokuRow(const SudokuRow &) = default;
  SudokuRow(SudokuRow &&) = default;

  BitSet &operator[](unsigned index) {
    assert(index < len_);
    return data_[index];
  }
  const BitSet &operator[](unsigned index) const {
    assert(index < len_);
    return data_[index];
  }

  unsigned Size() const { return len_; }

private:
  SudokuRow(BitSet *data, unsigned len) : data_(data), len_(len) {}

  BitSet *data_;
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

  const BitSet &operator[](unsigned index) const {
    assert(index < len_);
    return data_[index];
  }

  unsigned Size() const { return len_; }

private:
  ConstSudokuRow(const BitSet *data, unsigned len) : data_(data), len_(len) {}

  const BitSet *data_;
  unsigned len_;
  friend class Sudoku;
};

struct ChainsGraph {
  // Absolute position of the square inside of the puzzle.
  std::vector<unsigned> nodes;
  std::unordered_multimap<unsigned, unsigned> weak_links;
  std::unordered_multimap<unsigned, unsigned> strong_links;
};

class Sudoku {
public:
  /*! Construct an empty Sudoku of the given size and type.
   *
   * @param size Size of the Sudoku {9x9}, {16x16}, {25x25}.
   * @param type BASIC or DIAGONAL
   */
  Sudoku(unsigned size = 9, SudokuTypes type = BASIC);

  // Removed copy constructor.
  Sudoku(const Sudoku &) = delete;
  // Removed move constructor.
  Sudoku(Sudoku &&) = delete;
  // Removed assignment operator
  Sudoku &operator=(const Sudoku &) = delete;
  // Removed move assignment operator.
  Sudoku &operator=(Sudoku &&) = delete;

  /*! Build a strong/weak link graph of number possibilities
   *
   * @return The built graph.
   */
  ChainsGraph GetChains(unsigned number) const;

  /*! Build a weak link graph of squares with two possibilities.
   *
   * @return The built graph.
   */
   ChainsGraph GetPairChains() const;

  bool PruneNumbersSeenFrom(const std::vector<unsigned>& path, unsigned number);

  void SetSolution(const Sudoku* solution) {
    solution_ = solution;
  }

  bool HasSolution() const {
    return solution_ != nullptr;
  }

  bool CheckAgainstSolution() {
    if (solution_ == nullptr) return true;

    for (unsigned i = 0; i < Size(); i++) {
      for (unsigned j = 0; j < Size(); j++) {
        if (data_[i*Size()+j].HasSingletonValue()) {
          if (data_[i*Size()+j].SingletonValue() != solution_->data_[i*Size() + j].SingletonValue())
            return false;
        }
        if (!data_[i*Size() + j].IsBitSet(solution_->data_[i*Size() + j].SingletonValue()))
          return false;
      }
    }

    return true;
  }

  std::string Serialize() const;
  void Deserialize(const std::string& data);

  /*! Square bracket operator to allow for 2D access.
   *
   * @param index Row index to return.
   * @return A wrapper object around a row in the puzzle.
   */
  SudokuRow operator[](unsigned index) {
    return SudokuRow(&data_[index*Size()], Size());
  }
  /*! Square bracket operator to allow for 2D access.
   *
   * @param index Row index to return.
   * @return A const wrapper object around a row in the puzzle.
   */
  ConstSudokuRow operator[](unsigned index) const {
    return ConstSudokuRow(&data_[index*Size()], Size());
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
  std::unordered_set<UniqueBlock *> ChangedBlocks() const;

  //! Return the size of the Sudoku.
  unsigned Size() const { return size_; }

  //! Return a const reference to the list of blocks.
  const std::vector<UniqueBlock> &Blocks() const { return checks_; }
  //! Return a reference to the list of blocks.
  std::vector<UniqueBlock> &Blocks() { return checks_; }
  //! Return a const reference to the row blocks.
  const std::vector<UniqueBlock *> &GetRowBlocks() const {
    return row_checks_;
  }
  //! Return a const reference to the column blocks.
  const std::vector<UniqueBlock *> &GetColBlocks() const {
    return col_checks_;
  }

  //! Return whether all squares are set.
  bool IsSet() const;
  //! Return whether there is any conflict in the puzzle.
  bool HasConflict(); // TODO: return what the conflict is.

  //! Debug print the puzzle (output the current possibilities for each square).
  void DebugPrint(std::ostream &s);
  std::string DebugString();

  //! Solve fish for a given size and a number.
  void SolveFish(unsigned size, unsigned number);

  //! Solve finned fish for a given size and a number.
  void SolveFinnedFish(unsigned size, unsigned number);

  //! Solve X chains for a given length and a number.
  void SolveXChains(unsigned length, unsigned number);

  //! Solve XY chains for a given number.
  void SolveXYChains();

private:
  std::vector<UniqueBlock> checks_;
  std::vector<UniqueBlock *> row_checks_;
  std::vector<UniqueBlock *> col_checks_;
  std::vector<BitSet> data_;
  std::vector<BitSet> data_copy_;
  std::vector<std::vector<std::vector<UniqueBlock *>>> block_mapping_;

  unsigned size_;
  const Sudoku* solution_;
  SudokuTypes puzzle_type_;

  void MakeCopy() { data_copy_ = data_;}
  void SwapCopy() {
    for (unsigned i = 0; i < Size()*Size(); i++) {
      data_[i] = data_copy_[i];
    }
  }

  void SetupCheckers(unsigned size, SudokuTypes type);

  unsigned NumberOfSharedBlocks(std::pair<unsigned, unsigned> l,
                            std::pair<unsigned, unsigned> r) const;

  bool dfs_traverse(const std::vector<ChainsGraph>& g, const std::function<bool
      (const std::vector<unsigned>&)> &cb, std::vector<unsigned> &path,
                    unsigned number, unsigned next_number);

  bool dfs_traverse(const std::vector<ChainsGraph>& g, const std::function<bool
                                                                           (const
                                                              std::vector<unsigned>&)> &cb,
               unsigned number);

  void dfs_traverse(const ChainsGraph& g, unsigned length,
                    const std::function<void(const std::vector<unsigned>&)>
                        &cb);

  void dfs_traverse(const ChainsGraph& g, unsigned length,
                    const std::function<void(const std::vector<unsigned>&)> &cb,
                    std::vector<unsigned> &path, bool weak);

  friend std::vector<std::vector<std::vector<UniqueBlock *>>> &
  TestGetMappings(Sudoku &s);

  friend std::istream &operator>>(std::istream &s, Sudoku &puzzle);
};
} // namespace sudoku

#endif // SUDOKU_SUDOKU_H
