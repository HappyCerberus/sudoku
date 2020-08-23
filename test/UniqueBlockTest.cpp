/* (c) 2020 RNDr. Simon Toth (happy.cerberus@gmail.com) */

#include "../src/core/UniqueBlock.h"
#include "../src/core/BitSet.h"
#include <catch2/catch.hpp>

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

TEST_CASE("UniqueBlock : Simple", "") {
    SimpleBlock data(3u,3u);
    UniqueBlock block{data.block_data, 3u};

    REQUIRE(block.IsCompleteBlock());
    REQUIRE(block.Size() == 3);
    REQUIRE(block.Max() == 3);
}

TEST_CASE("UniqueBlock : HasConflict", "") {
    SimpleBlock data(3u,3u);
    UniqueBlock block{data.block_data, 3u};

    REQUIRE(!block.HasConflict());
    data.data[0] = BitSet::SingleBit(3u, 1u);
    REQUIRE(!block.HasConflict());
    data.data[1] = BitSet::SingleBit(3u, 1u);
    REQUIRE(block.HasConflict());
}

TEST_CASE("UniqueBlock : Prune", "") {
    SimpleBlock data(3u,3u);
    UniqueBlock block{data.block_data, 3u};

    REQUIRE((data.data[0].IsBitSet(2) && data.data[1].IsBitSet(2) && data.data[2].IsBitSet(2)));
    block.Prune(2, BitSet::Empty(3u));
    REQUIRE((!data.data[0].IsBitSet(2) && !data.data[1].IsBitSet(2) && !data.data[2].IsBitSet(2)));
    block.Prune(1, BitSet::SingleBit(3u, 2));
    REQUIRE((!data.data[0].IsBitSet(1) && data.data[1].IsBitSet(1) && !data.data[2].IsBitSet(1)));
}


TEST_CASE("UniqueBlock : NumberPositions", "") {
    SimpleBlock data(3u,3u);
    UniqueBlock block{data.block_data, 3u};

    BitSet set = block.NumberPositions(2);
    REQUIRE(set.CountSet() == 3);
    data.data[0] -= 2;
    set = block.NumberPositions(2);
    REQUIRE(set.CountSet() == 2);
    REQUIRE(!set.IsBitSet(1));
    REQUIRE(set.IsBitSet(2));
    REQUIRE(set.IsBitSet(3));
}


} // namespace sudoku