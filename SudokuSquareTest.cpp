// (c) 2020 RNDr. Simon Toth (happy.cerberus@gmail.com)

#include "SudokuSquare.h"
#include "gtest/gtest.h"

namespace {
    TEST(SudokuSquareTest, Empty) {
        SudokuSquare<uint16_t> s(9);
        EXPECT_EQ(s.CountPossible(), 9) << "With 9 as maximum value, we should have 9 possible values just after "
                                           "initialization.";
        EXPECT_EQ(s.Value(), 0) << "Multiple possibilities should return 0 as a value.";
    }
}