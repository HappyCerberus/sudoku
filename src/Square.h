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
  constexpr Square(const Square &r) noexcept = default;
  // Default move constructor.
  constexpr Square(Square &&r) noexcept = default;

  /*! Construct a square with all possibilities present
   *
   * @param max_value Maximum number to support {9, 16, 25, 36, 49, 64}
   */
  constexpr explicit Square(unsigned max_value = 9) noexcept
      : data_(~((~ZERO64) << static_cast<uint64_t>(max_value))), orig_(ZERO64),
        max_(static_cast<uint8_t>(max_value)){
    assert(max_value <= 64);
  }

  /*! Construct a square with all possibilities present
   *
   * @param max_value Maximum number to support {9, 16, 25, 36, 49, 64}
   */
  constexpr explicit Square(size_t max_value = 9) noexcept
      : Square(static_cast<unsigned>(max_value)){};

  /*! Return whether the given number is available as possibility
   *
   * @param number The number to check.
   * @return True if still available, Fals if not.
   */
  constexpr bool IsPossible(unsigned number) const noexcept {
    return (data_ & SingleBit(number)) != 0;
  }

  /*! Return The number of possiblities still present.
   *
   * @return Number of possibilities.
   */
  constexpr unsigned CountPossible() const noexcept {
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
  constexpr bool IsSet() const noexcept { return CountPossible() == 1; }

  /*! Returns the set value.
   *
   * @return 0 if not set, the single possibility otherwise.
   */
  constexpr unsigned Value() const noexcept {
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
  constexpr unsigned Next(unsigned number) const noexcept {
    for (unsigned i = number + 1; i <= static_cast<unsigned>(max_); i++) {
      if (IsPossible(i))
        return i;
    }
    return 0;
  }

  /*! Resets the square to contain all possibilities.
   *
   */
  constexpr void Reset() noexcept {
    data_ = ~((~ZERO64) << static_cast<uint64_t>(max_));
    orig_ = ZERO64;
  }

  /*! Resets the square to contain zero possibilities.
   *
   */
  constexpr void ResetToEmpty() noexcept {
    data_ = ZERO64;
  }

  /*! Mark the square as not changed.
   *
   */
  constexpr void ResetChanged() noexcept { orig_ = data_; }

  /*! Returns whether the square has changed since last ResetChanged().
   *
   * @return True if changed, False if not.
   */
  constexpr bool HasChanged() const noexcept { return orig_ != data_; }

  /*! Returns whether there is a possibility intersection with the given square.
   *
   * @param r The square to intersect with.
   * @return True if the squares intersect in at least one possibility. False
   * otherwise.
   */
  constexpr bool HasIntersection(const Square &r) const noexcept {
    return (data_ & r.data_) != 0;
  }

  /*! Returns whether there are additional possibilities in the square other
   *  than the ones given.
   *
   * @param r The given possibilities.
   * @return True if the squares has additional possibilities. False
   * otherwise.
   */
  constexpr bool HasExtraPossibilities(const Square &r) const noexcept {
    return (data_ & r.data_) != data_;
  }

  /*! Intersect this square with another.
   *
   * @param r Square to intersect with.
   * @return this
   */
  constexpr Square &operator&=(const Square &r) noexcept {
    data_ &= r.data_;
    return *this;
  }

  /*! Intersect this square with another.
   *
   * @param r Square to intersect with.
   * @return the intersection result.
   */
  constexpr Square operator&(const Square &r) const noexcept{
    Square result(*this);
    result.data_ &= r.data_;
    return result;
  }

  // Default assignment operator.
  constexpr Square &operator=(const Square &r) noexcept= default;

  /*! Reset the content of the square to contain only one possibility.
   *
   * @param number The possibility to reset to.
   * @return this
   */
  constexpr Square &operator=(unsigned number) noexcept{
    data_ = SingleBit(number);
    return *this;
  }

  /*! Add a number as a possibility.
   *
   * @param number Number to add, indexed from 1.
   * @return this
   */
  constexpr Square &operator+=(unsigned number) noexcept {
    data_ |= SingleBit(number);
    return *this;
  }

  /*! Add a number as a possibility.
   *
   * @param number Number to add, indexed from 1.
   * @return A new Square with the possiblity added.
   */
  constexpr Square operator+(unsigned number) const noexcept {
    Square result(*this);
    result.data_ |= SingleBit(number);
    return result;
  }

  /*! Union the possibilities of this Square with another Square.
   *
   * @param r Square to union with.
   * @return this
   */
  constexpr Square &operator+=(Square r) noexcept {
    data_ |= r.data_;
    return *this;
  }

  /*! Union the possibilities of this Square with another Square.
   *
   * @param r Square to union with.
   * @return A new Square with the resulting union.
   */
  constexpr Square operator+(Square r) const noexcept {
    Square result(*this);
    result.data_ |= r.data_;
    return result;
  }

  /*! Remove a possibility from a Square
   *
   * @param number Number to remove.
   * @return this
   */
  constexpr Square &operator-=(unsigned number) noexcept {
    data_ &= ~SingleBit(number);
    return *this;
  }

  /*! Remove a possibility from a Square
   *
   * @param number Number to remove.
   * @return A new Square with the number removed.
   */
  constexpr Square operator-(unsigned number) const noexcept {
    Square result(*this);
    result.data_ &= ~SingleBit(number);
    return result;
  }

  /*! Remove all posibilities that are possible in the righthand square.
   *
   * @param r The square with posibilities to be removed.
   * @return this
   */
  constexpr Square &operator-=(Square r) noexcept {
    data_ &= ~r.data_;
    return *this;
  }

  /*! Remove all posibilities that are possible in the righthand square.
   *
   * @param r The square with posibilities to be removed.
   * @return A new Square with the possibilites removed.
   */
  constexpr Square operator-(Square r) const noexcept {
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
  constexpr bool operator ==(const Square& r) const noexcept {
    return data_ == r.data_;
  }

  /*! Generate a debug a string representing the possibilities in the square.
   *
   * @return Debug string in the following format [1..456..9] where 1,4,5,6,9
   * are possibilities.
   */
  std::string DebugString() const;

private:
  constexpr void OverrideValue(uint64_t data) noexcept {
    data_ = data;
  }

  constexpr static uint64_t SingleBit(unsigned number) noexcept {
    return (static_cast<uint64_t>(1)
            << (static_cast<uint64_t>(number) - static_cast<uint64_t>(1)));
  }

  uint64_t data_;
  uint64_t orig_;
  uint8_t max_;

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
