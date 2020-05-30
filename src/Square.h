// (c) 2020 RNDr. Simon Toth (happy.cerberus@gmail.com)

#ifndef SUDOKU_SQUARE_H
#define SUDOKU_SQUARE_H

#include "gtest/gtest_prod.h"
#include <cstdint>
#include <ostream>
#include <vector>

namespace sudoku {
template <typename T> class Square {
public:
  typedef T BackingType;

  Square() = delete;

  Square(unsigned max_value = 9)
      : data_(~((~0u) << max_value)), max_(max_value), changed_(true) {
  }

  Square(const Square<T>& r) = default;

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

  /*! Add a number as a possibility.
   *
   * @param number Number to add, indexed from 1.
   * @return this
   */
  Square<T>& operator+=(unsigned number) {
    if (data_ != (data_ | (1 << (number - 1)))) {
      changed_ = true;
    }
    data_ |= (1 << (number - 1));
    return *this;
  }

  /*! Add a number as a possibility.
   *
   * @param number Number to add, indexed from 1.
   * @return A new Square with the possiblity added.
   */
  Square<T> operator+(unsigned number) const {
    Square<T> result(*this);
    result.data_ |= (1 << (number - 1));
    return result;
  }

  /*! Union the possibilities of this Square with another Square.
   *
   * @param r Square to union with.
   * @return this
   */
  Square<T>& operator+=(Square<T> r) {
    if (data_ != (data_ | r.data_)) {
      changed_ = true;
    }
    data_ |= r.data_;
    return *this;
  }

  /*! Union the possibilities of this Square with another Square.
   *
   * @param r Square to union with.
   * @return A new Square with the resulting union.
   */
  Square<T> operator+(Square<T> r) const {
    Square<T> result(*this);
    result.data_ |= r.data_;
    return result;
  }

  /*! Remove a possibility from a Square
   *
   * @param number Number to remove.
   * @return this
   */
  Square<T>& operator-=(unsigned number) {
    if (data_ != (data_ & ~(1 << (number - 1)))) {
      changed_ = true;
    }
    data_ &= ~(1 << (number - 1));
    return *this;
  }

  /*! Remove a possibility from a Square
   *
   * @param number Number to remove.
   * @return A new Square with the number removed.
   */
  Square<T> operator-(unsigned number) const {
    Square<T> result(*this);
    result.data_ &= ~(1 << (number - 1));
    return result;
  }

  /*! Remove all posibilities that are possible in the righthand square.
   *
   * @param r The square with posibilities to be removed.
   * @return this
   */
  Square<T>& operator-=(Square<T> r) {
    if (data_ != (data_ & ~r.data_)) {
      changed_ = true;
    }
    data_ &= ~r.data_;
    return *this;
  }

  /*! Remove all posibilities that are possible in the righthand square.
   *
   * @param r The square with posibilities to be removed.
   * @return A new Square with the possibilites removed.
   */
  Square<T> operator-(Square<T> r) const {
    Square<T> result(*this);
    result.data_ &= ~r.data_;
    return result;
  }

  void ResetChanged() { changed_ = false; }

  bool HasChanged() const { return changed_; }

  bool HasIntersection(const Square<T> &r) const {
    return (data_ & r.data_) != 0;
  }

  void Intersect(const Square<T> &r) {
    if (data_ != (data_ & r.data_)) {
      changed_ = true;
    }
    data_ &= r.data_;
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

  T data_;
  uint8_t max_;
  bool changed_;

  FRIEND_TEST(BlockCheckerTest, recursive_set_find);
  FRIEND_TEST(BlockCheckerTest, recursive_number_find);
};

typedef Square<uint16_t> SquareType;
} // namespace sudoku

#endif // SUDOKU_SQUARE_H
