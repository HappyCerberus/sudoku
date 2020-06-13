// (c) 2020 RNDr. Simon Toth (happy.cerberus@gmail.com)

#ifndef SUDOKU_SQUARE_H
#define SUDOKU_SQUARE_H

//#include "gtest/gtest_prod.h"
#include <cassert>
#include <cstdint>
#include <iosfwd>
#include <string>
#include <vector>

namespace sudoku {
class Square {
  static const constexpr uint64_t ZERO64 = 0;

public:
  // No default constructor.
  Square() = delete;
  // Default copy constructor.
  Square(const Square &r) = default;
  // Default move constructor.
  Square(Square &&r) = default;

  /*! Construct a square with all possibilities present
   *
   * @param max_value Maximum number to support {9, 16, 25, 36, 49, 64}
   */
  explicit Square(unsigned max_value = 9)
      : data_(~((~ZERO64) << static_cast<uint64_t>(max_value))),
        max_(static_cast<uint8_t>(max_value)), changed_(true) {
    assert(max_value <= 64);
  }

  /*! Construct a square with all possibilities present
   *
   * @param max_value Maximum number to support {9, 16, 25, 36, 49, 64}
   */
  explicit Square(size_t max_value = 9)
      : Square(static_cast<unsigned>(max_value)){};

  /*! Return whether the given number is available as possibility
   *
   * @param number The number to check.
   * @return True if still available, Fals if not.
   */
  bool IsPossible(unsigned number) const {
    return (data_ & SingleBit(number)) != 0;
  }

  /*! Return The number of possiblities still present.
   *
   * @return Number of possibilities.
   */
  unsigned CountPossible() const {
    unsigned result = 0;
    for (unsigned i = 1; i <= static_cast<unsigned>(max_); i++) {
      if (IsPossible(i))
        result++;
    }
    return result;
  }

  /*! Returns whether only a single possibility is present.
   *
   * @return True if only one possibility is present, false otherwise.
   */
  bool IsSet() const { return CountPossible() == 1; }

  /*! Returns the set value.
   *
   * @return 0 if not set, the single possibility otherwise.
   */
  unsigned Value() const {
    if (CountPossible() != 1)
      return 0;
    for (unsigned i = 1; i <= static_cast<unsigned>(max_); i++) {
      if (IsPossible(i))
        return i;
    }
    // unreachable
    return 0;
  }

  /*! Convenience function that returns the next possible number.
   *
   * @param number The starting number to search from.
   * @return 0 if none other are possible, next possibility otherwise.
   */
  unsigned Next(unsigned number) const {
    for (unsigned i = number + 1; i <= static_cast<unsigned>(max_); i++) {
      if (IsPossible(i))
        return i;
    }
    return 0;
  }

  /*! Resets the square to contain all possibilities.
   *
   */
  void Reset() {
    if (data_ != ~((~ZERO64) << static_cast<uint64_t>(max_))) {
      changed_ = true;
    }
    data_ = ~((~ZERO64) << static_cast<uint64_t>(max_));
  }

  /*! Resets the square to contain zero possibilities.
   *
   */
  void ResetToEmpty() {
    if (data_ != ZERO64) {
      changed_ = true;
    }
    data_ = ZERO64;
  }

  /*! Mark the square as not changed.
   *
   */
  void ResetChanged() { changed_ = false; }

  /*! Returns whether the square has changed since last ResetChanged().
   *
   * @return True if changed, False if not.
   */
  bool HasChanged() const { return changed_; }

  /*! Returns whether there is a possibility intersection with the given square.
   *
   * @param r The square to intersect with.
   * @return True if the squares intersect in at least one possibility. False
   * otherwise.
   */
  bool HasIntersection(const Square &r) const { return (data_ & r.data_) != 0; }

  /*! Intersect this square with another.
   *
   * @param r Square to intersect with.
   * @return this
   */
  Square &operator&=(const Square &r) {
    if (data_ != (data_ & r.data_)) {
      changed_ = true;
    }
    data_ &= r.data_;
    return *this;
  }

  /*! Intersect this square with another.
   *
   * @param r Square to intersect with.
   * @return the intersection result.
   */
  Square operator&(const Square &r) const {
    Square result(*this);
    result.data_ &= r.data_;
    return result;
  }

  // Default assignment operator.
  Square &operator=(const Square &r) = default;

  /*! Reset the content of the square to contain only one possibility.
   *
   * @param number The possibility to reset to.
   * @return this
   */
  Square &operator=(unsigned number) {
    if (data_ != SingleBit(number)) {
      changed_ = true;
    }
    data_ = SingleBit(number);
    return *this;
  }

  /*! Add a number as a possibility.
   *
   * @param number Number to add, indexed from 1.
   * @return this
   */
  Square &operator+=(unsigned number) {
    if (data_ != (data_ | SingleBit(number))) {
      changed_ = true;
    }
    data_ |= SingleBit(number);
    return *this;
  }

  /*! Add a number as a possibility.
   *
   * @param number Number to add, indexed from 1.
   * @return A new Square with the possiblity added.
   */
  Square operator+(unsigned number) const {
    Square result(*this);
    result.data_ |= SingleBit(number);
    return result;
  }

  /*! Union the possibilities of this Square with another Square.
   *
   * @param r Square to union with.
   * @return this
   */
  Square &operator+=(Square r) {
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
  Square operator+(Square r) const {
    Square result(*this);
    result.data_ |= r.data_;
    return result;
  }

  /*! Remove a possibility from a Square
   *
   * @param number Number to remove.
   * @return this
   */
  Square &operator-=(unsigned number) {
    if (data_ != (data_ & ~SingleBit(number))) {
      changed_ = true;
    }
    data_ &= ~SingleBit(number);
    return *this;
  }

  /*! Remove a possibility from a Square
   *
   * @param number Number to remove.
   * @return A new Square with the number removed.
   */
  Square operator-(unsigned number) const {
    Square result(*this);
    result.data_ &= ~SingleBit(number);
    return result;
  }

  /*! Remove all posibilities that are possible in the righthand square.
   *
   * @param r The square with posibilities to be removed.
   * @return this
   */
  Square &operator-=(Square r) {
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
  Square operator-(Square r) const {
    Square result(*this);
    result.data_ &= ~r.data_;
    return result;
  }

  /*! Compare two sudoku squares.
   *
   * @param r Square to compare to.
   * @return True if both squares contain the same possibilities, False
   * otherwise.
   */
  bool operator ==(const Square& r) const {
    return data_ == r.data_;
  }

  /*! Generate a debug a string representing the possibilities in the square.
   *
   * @return Debug string in the following format [1..456..9] where 1,4,5,6,9
   * are possibilities.
   */
  std::string DebugString() const;

private:
  void OverrideValue(uint64_t data) {
    if (data != data_) {
      changed_ = true;
    }
    data_ = data;
  }

  static uint64_t SingleBit(unsigned number) {
    return (static_cast<uint64_t>(1)
            << (static_cast<uint64_t>(number) - static_cast<uint64_t>(1)));
  }

  uint64_t data_;
  uint8_t max_;
  bool changed_;

  friend void TestOverrideValue(Square &, uint64_t);

  // FRIEND_TEST(BlockCheckerTest, recursive_set_find);
  // FRIEND_TEST(BlockCheckerTest, recursive_number_find);

  friend std::ostream &operator<<(std::ostream &s, const Square &r);
};

/*! Ostream operator for sudoku square.
 *
 * @param s Stream to write to.
 * @param r Square to serialize.
 * @return stream written to
 */
std::ostream &operator<<(std::ostream &s, const Square &r);

} // namespace sudoku

#endif // SUDOKU_SQUARE_H
