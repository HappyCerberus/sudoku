// (c) 2020 RNDr. Simon Toth (happy.cerberus@gmail.com)

#ifndef SUDOKU_SUDOKUSQUARE_H
#define SUDOKU_SUDOKUSQUARE_H

#include "gtest/gtest_prod.h"
#include <cstdint>
#include <ostream>
#include <vector>

template <typename T> class SudokuSquare {
public:
  typedef T BackingType;

  SudokuSquare() = delete;

  SudokuSquare(unsigned max_value = 9)
      : max_(max_value), data_(0), changed_(true) {
    data_ = ~((~0u) << max_value);
  }

  bool IsPossible(unsigned number) const {
    return (data_ & (1 << (number - 1))) != 0;
  }

  // C++ 20 has popcount
  unsigned CountPossible() const {
    unsigned result = 0;
    for (unsigned i = 1; i <= max_; i++) {
      if (IsPossible(i))
        result++;
    }
    return result;
  }

  bool IsSet() const { return CountPossible() == 1; }

  unsigned Value() const {
    if (CountPossible() != 1)
      return 0;
    for (unsigned i = 1; i <= max_; i++) {
      if (IsPossible(i))
        return i;
    }
    // unreachable
    return 0;
  }

  unsigned Next(unsigned number) const {
    for (unsigned i = number + 1; i <= max_; i++) {
      if (IsPossible(i))
        return i;
    }
    return 0;
  }

  void Reset() {
    if (data_ != ~((~0u) << max_)) {
      changed_ = true;
    }
    data_ = ~((~0u) << max_);
  }

  void SetToEmpty() {
    if (data_ != 0u) {
      changed_ = true;
    }
    data_ = 0u;
  }

  void Set(unsigned number) {
    if (data_ != (1 << (number - 1))) {
      changed_ = true;
    }
    data_ = (1 << (number - 1));
  }

  void Add(unsigned number) {
    if (data_ != (data_ | (1 << (number - 1)))) {
      changed_ = true;
    }
    data_ |= (1 << (number - 1));
  }

  void Remove(unsigned number) {
    if (data_ != (data_ & ~(1 << (number - 1)))) {
      changed_ = true;
    }
    data_ &= ~(1 << (number - 1));
  }

  void ResetChanged() { changed_ = false; }

  bool HasChanged() const { return changed_; }

  void Union(const SudokuSquare<T> &r) {
    if (data_ != (data_ | r.data_)) {
      changed_ = true;
    }
    data_ |= r.data_;
  }

  bool HasIntersection(const SudokuSquare<T> &r) const {
    return (data_ & r.data_) != 0;
  }

  void Intersect(const SudokuSquare<T> &r) {
    if (data_ != (data_ & r.data_)) {
      changed_ = true;
    }
    data_ &= r.data_;
  }

  void Minus(const SudokuSquare<T> r) {
    if (data_ != (data_ & ~r.data_)) {
      changed_ = true;
    }
    data_ &= ~r.data_;
  }

  void DebugPrint(std::ostream &s) const {
    // [1.3.5..8.]
    s << "[";
    for (size_t i = 0; i < max_; i++) {
      if (IsPossible(i + 1)) {
        if (i + 1 > 9) {
          s << static_cast<char>(i - 9 + 'A');
        } else {
          s << i + 1;
        }
      } else {
        s << ".";
      }
    }
    s << "]";
  }

private:
  void OverrideValue(T data) {
    if (data != data_) {
      changed_ = true;
    }
    data_ = data;
  }

  unsigned max_;
  T data_;
  bool changed_;

  FRIEND_TEST(BlockCheckerTest, recursive_set_find);
  FRIEND_TEST(BlockCheckerTest, recursive_number_find);
};

typedef SudokuSquare<uint16_t> SquareType;

#endif // SUDOKU_SUDOKUSQUARE_H
