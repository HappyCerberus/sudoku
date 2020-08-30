#include <catch2/catch.hpp>
#include "../src/killer/KillerBlock.h"

namespace sudoku {

namespace {
struct SimpleBlock {
    SimpleBlock(unsigned size, unsigned max) {
        for (unsigned i = 0; i < size; i++) {
            data.push_back(BitSet::SudokuSquare(max));
        }
        for (unsigned i = 0; i < size; i++) {
            block_data.push_back(&data[i]);
        }
    }
    std::vector<BitSet> data;
    std::vector<BitSet*> block_data;
};
}

std::vector<bool> &TestGetPossibleSets(KillerBlock &s) { return s.possible_sets_; }

TEST_CASE("Killer Block : union", "[killer]") {
    SimpleBlock two(2u,9u);
    SimpleBlock three(3u,9u);

    KillerBlock k1(two.block_data, 9u, 14u);
    BitSet u1 = k1.UnionSumSet();
    CHECK(u1.CountSet() == 4);
    CHECK(u1.IsBitSet(5));
    CHECK(u1.IsBitSet(6));
    CHECK(u1.IsBitSet(8));
    CHECK(u1.IsBitSet(9));

    KillerBlock k2(three.block_data, 9u, 6u);
    BitSet u2 = k2.UnionSumSet();
    CHECK(u2.CountSet() == 3);
    CHECK(u2.IsBitSet(1));
    CHECK(u2.IsBitSet(2));
    CHECK(u2.IsBitSet(3));

    KillerBlock k3(three.block_data, 9u, 9u);
    BitSet u3 = k3.UnionSumSet();
    CHECK(u3.CountSet() == 6);
    CHECK(!u3.IsBitSet(7));
    CHECK(!u3.IsBitSet(8));
    CHECK(!u3.IsBitSet(9));

    TestGetPossibleSets(k3)[2] = false;
    u3 = k3.UnionSumSet();
    CHECK(u3.CountSet() == 5);
    CHECK(!u3.IsBitSet(6));
    CHECK(!u3.IsBitSet(7));
    CHECK(!u3.IsBitSet(8));
    CHECK(!u3.IsBitSet(9));

    TestGetPossibleSets(k3)[1] = false;
    u3 = k3.UnionSumSet();
    CHECK(u3.CountSet() == 3);
    CHECK(u3.IsBitSet(2));
    CHECK(u3.IsBitSet(3));
    CHECK(u3.IsBitSet(4));
}

TEST_CASE("Killer Block : Prune", "[killer]") {
    SimpleBlock three(3u, 9u);
    KillerBlock k1(three.block_data, 9u, 15u);
    CHECK(TestGetPossibleSets(k1).size() == 8u);

    three.data[0] -= 8;
    three.data[1] -= 8;
    three.data[2] -= 8;
    k1.PruneSumSets();
    auto &v = TestGetPossibleSets(k1);
    CHECK(std::count(v.begin(), v.end(), true) == 5u);
}


}