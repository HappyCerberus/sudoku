// (c) 2020 RNDr. Simon Toth (happy.cerberus@gmail.com)

#include "Square.h"
#include "gtest/gtest.h"

namespace {
TEST(SudokuSquareTest, Empty) {
  sudoku::Square s(9u);
  EXPECT_EQ(s.CountPossible(), 9)
      << "With 9 as maximum value, we should have 9 possible values just after "
         "initialization.";
  EXPECT_EQ(s.Value(), 0)
      << "Multiple possibilities should return 0 as a value.";
  for (unsigned i = 1; i <= 9; i++) {
    EXPECT_TRUE(s.IsPossible(i)) << "All values should be possible :" << i;
  }
  EXPECT_FALSE(s.IsSet());
  for (unsigned i = 0; i < 9; i++) {
    EXPECT_EQ(s.Next(i), i + 1);
  }

  s = 3;
  EXPECT_EQ(s.Value(), 3) << "Manually set to 3";
  EXPECT_TRUE(s.IsPossible(3));
  EXPECT_EQ(s.CountPossible(), 1);
  EXPECT_TRUE(s.IsSet());
  EXPECT_EQ(s.Next(0), 3);
  EXPECT_EQ(s.Next(3), 0);

  s.Reset();
  EXPECT_EQ(s.CountPossible(), 9)
      << "With 9 as maximum value, we should have 9 possible values just after "
         "initialization.";
  EXPECT_EQ(s.Value(), 0)
      << "Multiple possibilities should return 0 as a value.";
  for (unsigned i = 1; i <= 9; i++) {
    EXPECT_TRUE(s.IsPossible(i)) << "All values should be possible :" << i;
  }
  EXPECT_FALSE(s.IsSet());
  for (unsigned i = 0; i < 9; i++) {
    EXPECT_EQ(s.Next(i), i + 1);
  }
}
} // namespace