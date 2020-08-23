// (c) 2020 RNDr. Simon Toth (happy.cerberus@gmail.com)

#ifndef CORE_BITSET_H_
#define CORE_BITSET_H_

#include <bit>
#include <compare>
#include <cstdint>
#include <iosfwd>
#include <cassert>
#include <iterator>

namespace sudoku {

class BitSet {
 public:
  // All default constructors.
  constexpr BitSet() noexcept = default;
  constexpr BitSet(const BitSet &) noexcept = default;
  constexpr BitSet(BitSet &&) noexcept = default;
  constexpr BitSet &operator=(const BitSet &) noexcept = default;
  constexpr BitSet &operator=(BitSet &&) noexcept = default;

  [[nodiscard]] constexpr auto operator<=>(const BitSet &rhs) const noexcept = default;

  [[nodiscard]] static constexpr BitSet SudokuSquare(unsigned max_value = 9) noexcept {
    assert(max_value <= 64);
    BitSet result;
    result.data_ = ~((~UINT64_C(0)) << static_cast<uint64_t>(max_value));
    result.max_ = static_cast<uint8_t>(max_value);
    return result;
  }

  [[nodiscard]] static constexpr BitSet Set(unsigned max_value = 9, unsigned bits_set = 9) noexcept {
    assert(max_value <= 64);
    BitSet result;
    result.data_ = ~((~UINT64_C(0)) << static_cast<uint64_t>(bits_set));
    result.max_ = static_cast<uint8_t>(max_value);
    return result;
  }

  [[nodiscard]] static constexpr BitSet Empty(unsigned max_value = 9) noexcept {
    assert(max_value <= 64);
    BitSet result;
    result.data_ = UINT64_C(0);
    result.max_ = static_cast<uint8_t>(max_value);
    return result;
  }

  /*! Construct a single-bit BitSet.
	 * @param number The ordinal number of the bit to be set.
	 * @return BitSet with a single bit set.
	 */
  [[nodiscard]] static constexpr BitSet SingleBit(unsigned max_value, unsigned number) noexcept {
    return BitSet(SingleBitValue(number), static_cast<uint8_t>(max_value));
  }

  /*! Set minus operation
	 * @param lhs Left hand side of the operator.
	 * @param rhs Right hand side of the operator.
	 * @return Result of the set minus operation.
	 */
  [[nodiscard]] friend constexpr BitSet operator-(const BitSet &lhs, const BitSet &rhs) noexcept {
    return BitSet(lhs.data_ & ~rhs.data_, lhs.max_);
  }

  /*! Set union operation
	 * @param lhs Left hand side of the operator.
	 * @param rhs Right hand side of the operator.
	 * @return Result of the set union operation.
	 */
  [[nodiscard]] friend constexpr BitSet operator|(const BitSet &lhs, const BitSet &rhs) noexcept {
    return BitSet(lhs.data_ | rhs.data_, lhs.max_);
  }

  /*! Set intersection operation
	 * @param lhs Left hand side of the operator.
	 * @param rhs Right hand side of the operator.
	 * @return Result of the set intersection operation.
	 */
  [[nodiscard]] friend constexpr BitSet operator&(const BitSet &lhs, const BitSet &rhs) noexcept {
    return BitSet(lhs.data_ & rhs.data_, lhs.max_);
  }

  /*! Set minus operation
	 * @param rhs Right hand side of the operator.
	 * @return Result of the set minus operation.
	 */
  constexpr BitSet &operator-=(const BitSet &rhs) noexcept {
    data_ &= ~rhs.data_;
    return *this;
  }

  /*! Set union operation
	 * @param rhs Right hand side of the operator.
	 * @return Result of the set union operation.
	 */
  constexpr BitSet &operator|=(const BitSet &rhs) noexcept {
    data_ |= rhs.data_;
    return *this;
  }

  /*! Set intersection operation
   * @param rhs Right hand side of the operator.
	 * @return Result of the set intersection operation.
	 */
  constexpr BitSet &operator&=(const BitSet &rhs) noexcept {
    data_ &= rhs.data_;
    return *this;
  }

  /*! Add a number as a possibility.
   *
   * @param number Number to add, indexed from 1.
   * @return this
   */
  constexpr BitSet &operator+=(unsigned number) noexcept {
    data_ |= SingleBitValue(number);
    return *this;
  }

  /*! Add a number as a possibility.
   *
   * @param number Number to add, indexed from 1.
   * @return A new Square with the possiblity added.
   */
  [[nodiscard]] constexpr BitSet operator+(unsigned number) const noexcept {
    BitSet result(*this);
    result.data_ |= SingleBitValue(number);
    return result;
  }

  /*! Remove a possibility from a Square
   *
   * @param number Number to remove.
   * @return this
   */
  constexpr BitSet &operator-=(unsigned number) noexcept {
    data_ &= ~SingleBitValue(number);
    return *this;
  }

  /*! Remove a possibility from a Square
   *
   * @param number Number to remove.
   * @return A new Square with the number removed.
   */
  [[nodiscard]] constexpr BitSet operator-(unsigned number) const noexcept {
    BitSet result(*this);
    result.data_ &= ~SingleBitValue(number);
    return result;
  }

  /*! Returns whether there is an intersection with the given bitset.
   *
   * @param rhs The bitset to check intersection with.
   * @return True if the bitsets intersect on at least one bit. False otherwise.
   */
  [[nodiscard]] constexpr bool HasIntersection(const BitSet &rhs) const noexcept {
    return (data_ & rhs.data_) != 0;
  }

  /*! Returns whether this bitset contains bits not contained within the given bitset.
	 * @param rhs The bitset to check against.
	 * @return True if this bitset contains bits not contained within the given bitset, false otherwise.
	 */
  [[nodiscard]] constexpr bool HasAdditionalBits(const BitSet &rhs) const noexcept {
    return (data_ & rhs.data_) != data_;
  }

  /*! Counts number of bits set.
	 * @return number of bits set.
	 */
  [[nodiscard]] constexpr unsigned CountSet() const noexcept {
    return static_cast<unsigned>(std::popcount(data_));
  }

  /*! If a single bit is set, returns the ordinal position of the bit.
	 *  Unspecified, if more then one bit is set.
	 * @return Ordinal position of the bit.
	 */
  [[nodiscard]] constexpr unsigned SingletonValue() const noexcept {
    return static_cast<unsigned>(std::countr_zero(data_)) + 1u;
  }

  /*! Check whether only a single bit is set.
	 * @returns true if only a single bit is set.
	 */
  [[nodiscard]] constexpr bool HasSingletonValue() const noexcept {
    return std::has_single_bit(data_);
  }

  /*! Check whether the given bit is set.
	 * @returns true if set, false otherwise
	 */
  [[nodiscard]] constexpr bool IsBitSet(unsigned bit) const noexcept {
    return (data_ & SingleBitValue(bit)) != 0;
  }

  /*! Generate a debug a string representing the possibilities in the bitset.
   *
   * @return Debug string in the following format [1..456..9] where 1,4,5,6,9
   * are possibilities.
   */
  std::string DebugString() const;

  /*! Return the ordinal value of the maximum bit that can be set.
   * @return Ordinal value of the maximum bit that can be set.
   */
  [[nodiscard]] constexpr unsigned Max() const noexcept { 
	  return max_; 
	}

  friend std::ostream& operator << (std::ostream& s, const BitSet& b);

protected:
  /*! Serialize the value of this bitset into a stream.
   * @param s Stream to serialize to.
   */
  void Serialize(std::ostream &s) const;

  /*! Deserialize the value of this bitset into a stream.
   * @param s Stream to deserialize from.
   */
  void Deserialize(std::istream &s, unsigned max);

  /*! Explicit from value constructor.
	 * @param value The value to set this bitset to.
	 */
  explicit constexpr BitSet(uint64_t value) noexcept : data_(value) {}

  /*! Generate a value with a single bit set.
	 * @param number The ordinal of the bit to be set.
	 * @return Value with a single bit set.
	 */
  [[nodiscard]] static constexpr uint64_t SingleBitValue(unsigned number) noexcept {
    return UINT64_C(1) << (static_cast<uint64_t>(number) - UINT64_C(1));
  }

 private:
  uint64_t data_ = 0;
  uint8_t max_ = 0;

  constexpr BitSet(uint64_t data, uint8_t max) noexcept : data_(data), max_(max) {}

  friend constexpr BitSet NextSet(const BitSet& set) noexcept;
  friend class Sudoku;
};

inline constexpr BitSet NextSet(const BitSet& set) noexcept {
    // Get the rightmost 1 bit and add it to the number.
    // This will give us the next one bit that needs to be set.
	uint64_t shifted = set.data_ + (1 << std::countr_zero(set.data_));
	if (shifted < set.data_ || std::bit_floor(shifted) > (UINT64_C(1) << (static_cast<uint64_t>(set.max_-1))))
		return BitSet::Empty(set.max_);
	// XOR to get the changed bits (all the consecutive ones, that flipped to 0s and the new left 1).
	uint64_t isolated = set.data_^shifted;
	// shift the pattern to the right, discarding the two leftmost bits
	// we discard two bits because, isolated has all the changed bits (which already is one extra)
	// and shifted already has the new left bit added, which is the second bit
	isolated /= (UINT64_C(1) << std::countr_zero(set.data_));
	isolated >>= 2;
	return BitSet(shifted | isolated, set.max_);
}

struct BitIterator {
    using iterator_category = std::input_iterator_tag;
    using value_type = unsigned;
    using difference_type = void;
    using pointer = unsigned*;
    using reference = unsigned&;

    constexpr BitIterator(const sudoku::BitSet* set) : set_(set), current_(0u) { NextBit(); }
    constexpr BitIterator(const sudoku::BitSet* set, unsigned val) : set_(set), current_(val) {}
    constexpr BitIterator(const BitIterator& rhs) = default;
    [[nodiscard]] constexpr BitIterator& operator=(const BitIterator& rhs) noexcept = default;

    [[nodiscard]] constexpr bool operator != (const BitIterator& rhs) const noexcept {
        return current_ != rhs.current_;
    }

    friend constexpr BitIterator& operator++(BitIterator& it) noexcept {
        it.NextBit();
        return it;
    }
    friend constexpr BitIterator operator++(BitIterator& it, int) noexcept {
        BitIterator result(it);
        it.NextBit();
        return result;
    }

    [[nodiscard]] constexpr value_type operator*() const noexcept { return current_; }

  private:
    constexpr void NextBit() noexcept {
      current_++;
      while (current_ <= set_->Max() && !set_->IsBitSet(current_))
        current_++;
    }

    const sudoku::BitSet* set_;
    unsigned current_;
};

struct SetIterator {
    using iterator_category = std::input_iterator_tag;
    using value_type = sudoku::BitSet;
    using difference_type = void;
    using pointer = sudoku::BitSet*;
    using reference = sudoku::BitSet&;

    constexpr SetIterator(const sudoku::BitSet set) noexcept : current_(set) {}
    constexpr SetIterator(const SetIterator&) noexcept = default;
    [[nodiscard]] constexpr SetIterator& operator=(const SetIterator&) noexcept = default;

    [[nodiscard]] constexpr bool operator != (const SetIterator& rhs) const noexcept {
        return current_ != rhs.current_;
    }

    friend constexpr SetIterator& operator++(SetIterator& it) noexcept {
        it.current_ = sudoku::NextSet(it.current_);
        return it;
    }
    friend constexpr SetIterator operator++(SetIterator& it, int) noexcept {
        SetIterator result(it);
        it.current_ = sudoku::NextSet(it.current_);
        return result;
    }

    constexpr value_type operator*() const { return current_; }
    constexpr const sudoku::BitSet* operator->() const { return &current_; }

  private:
    sudoku::BitSet current_;    
};

struct BitSetBits {
    constexpr BitSetBits(const sudoku::BitSet* set) noexcept : set_(set) {}
    [[nodiscard]] constexpr auto begin() const noexcept { return BitIterator(set_); };
    [[nodiscard]] constexpr auto end() const noexcept { return BitIterator(set_, set_->Max()+1); };
  private:
    const sudoku::BitSet* set_;
};

struct BitSetSets {
    constexpr BitSetSets(unsigned range = 9u, unsigned size = 9u) noexcept : range_(range), size_(size) {}
    [[nodiscard]] constexpr auto begin() const noexcept { return SetIterator(sudoku::BitSet::Set(range_, size_)); };
    [[nodiscard]] constexpr auto end() const noexcept { return SetIterator(sudoku::BitSet::Empty(range_)); };
  private:
    unsigned range_;
    unsigned size_;
};

}  // namespace sudoku

#endif  // SUDOKU_BITSET_H_
