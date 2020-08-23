#include <catch2/catch.hpp>
#include "../src/core/BitSet.h"

namespace sudoku {

constexpr BitSet SetupBitSet(uint64_t value) {
	BitSet a;
	for (unsigned i = 0; i < 64; i++) {
		if ((value & (UINT64_C(1) << static_cast<uint64_t>(i))) != 0) {
			a |= BitSet::SingleBit(9, i+1u);
		}
	}
	return a; 
}

TEST_CASE("BitSet tests", "[bitset]") {
  STATIC_REQUIRE(BitSet::SingleBit(9, 8).SingletonValue() == 8);
  STATIC_REQUIRE(BitSet::SingleBit(9, 8).HasSingletonValue());
  STATIC_REQUIRE(BitSet::SingleBit(9, 8).CountSet() == 1);

  constexpr BitSet a = SetupBitSet(UINT64_C(0b111));
  STATIC_REQUIRE(a.CountSet() == 3);
  STATIC_REQUIRE(!a.HasSingletonValue());

  constexpr BitSet b = SetupBitSet(UINT64_C(0b010));
  STATIC_REQUIRE(b.CountSet() == 1);
  STATIC_REQUIRE(b.HasSingletonValue());
  STATIC_REQUIRE(b.SingletonValue() == 2);

  constexpr BitSet c = a-b;
  STATIC_REQUIRE(c == SetupBitSet(UINT64_C(0b101)));
  STATIC_REQUIRE((c | b) == a);

  constexpr BitSet d = SetupBitSet(UINT64_C(0b011));
  STATIC_REQUIRE(d.CountSet() == 2);
  STATIC_REQUIRE(!d.HasSingletonValue());

  constexpr BitSet e = SetupBitSet(UINT64_C(0b110));
  STATIC_REQUIRE(e.CountSet() == 2);
  STATIC_REQUIRE(!e.HasSingletonValue());

  constexpr BitSet f = d & e;
  STATIC_REQUIRE(f.CountSet() == 1);
  STATIC_REQUIRE(f.HasSingletonValue());
  STATIC_REQUIRE(f.SingletonValue() == 2);

  STATIC_REQUIRE(f < a);
  STATIC_REQUIRE(c < a);
  STATIC_REQUIRE((e > d && e > f && d > f));

  STATIC_REQUIRE(e.IsBitSet(2));
  STATIC_REQUIRE(!e.IsBitSet(1));
  STATIC_REQUIRE(!f.HasAdditionalBits(e));
  STATIC_REQUIRE(e.HasAdditionalBits(c));
  STATIC_REQUIRE(!f.HasIntersection(c));
  STATIC_REQUIRE(e.HasIntersection(c));

  constexpr BitSet i = BitSet::SudokuSquare(1);
  STATIC_REQUIRE(i.CountSet() == 1);
  STATIC_REQUIRE(i.SingletonValue() == 1);
  constexpr BitSet j = BitSet::SudokuSquare(9);
  STATIC_REQUIRE(j.CountSet() == 9);
  constexpr BitSet k = BitSet::Empty();
  STATIC_REQUIRE(k.CountSet() == 0);
  constexpr BitSet l = BitSet::Set(9, 3);
  STATIC_REQUIRE(l.CountSet() == 3);
  STATIC_REQUIRE(l.IsBitSet(1));
  STATIC_REQUIRE(l.IsBitSet(2));
  STATIC_REQUIRE(l.IsBitSet(3));

  constexpr BitSet m = BitSet::SingleBit(9, 4) + 5;
  STATIC_REQUIRE(m.CountSet() == 2);
  STATIC_REQUIRE(m.IsBitSet(4));
  STATIC_REQUIRE(m.IsBitSet(5));
  constexpr BitSet n = BitSet::Set(9, 4) - 2;
  STATIC_REQUIRE(n.CountSet() == 3);
  STATIC_REQUIRE(n.IsBitSet(1));
  STATIC_REQUIRE(n.IsBitSet(3));
  STATIC_REQUIRE(n.IsBitSet(4));
}

namespace {
constexpr unsigned CountBits(const BitSet& s) noexcept {
  unsigned bits = 0;
  for (unsigned bit : BitSetBits(&s)) {
    (void)bit;
    bits++;
  }
  return bits;
}
}

TEST_CASE("BitIterator tests 1", "[bititer]") {
  static constexpr BitSet a = BitSet::Set(9, 3);
  constexpr auto iter = BitSetBits{&a}.begin();
  STATIC_REQUIRE(*iter == 1);
  STATIC_REQUIRE(CountBits(a) == 3);
}

TEST_CASE("BitIterator tests 2", "[nextbit]") {
  constexpr BitSet a = BitSet::SudokuSquare(9);
  STATIC_REQUIRE(CountBits(a) == 9);
  constexpr BitSet b = BitSet::Set(9,5);
  STATIC_REQUIRE(CountBits(b) == 5);
  constexpr BitSet c = BitSet::SingleBit(9, 7);
  STATIC_REQUIRE(CountBits(c) == 1);
}

namespace {
  constexpr auto Next(auto it) {
    return ++it;
  }
}

TEST_CASE("BitSet Iteration tests", "[nextset]") {
  constexpr auto iter1 = BitSetSets(9, 5).begin();
  constexpr auto iter2 = Next(iter1);
  STATIC_REQUIRE(iter2->CountSet() == 5);
  STATIC_REQUIRE(iter2->IsBitSet(6));
  STATIC_REQUIRE(iter2->IsBitSet(4));
  STATIC_REQUIRE(iter2->IsBitSet(3));
  STATIC_REQUIRE(iter2->IsBitSet(2));
  STATIC_REQUIRE(iter2->IsBitSet(1));
  constexpr auto iter3 = Next(iter2);
  STATIC_REQUIRE(iter3->CountSet() == 5);
  STATIC_REQUIRE(iter3->IsBitSet(6));
  STATIC_REQUIRE(iter3->IsBitSet(5));
  STATIC_REQUIRE(iter3->IsBitSet(3));
  STATIC_REQUIRE(iter3->IsBitSet(2));
  STATIC_REQUIRE(iter3->IsBitSet(1));
}

}