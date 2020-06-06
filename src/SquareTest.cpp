// (c) 2020 RNDr. Simon Toth (happy.cerberus@gmail.com)

#include "Square.h"
#include "gtest/gtest.h"

namespace {
TEST(SudokuSquareTest, Empty) {
  sudoku::Square s(9u);
  EXPECT_EQ(s.CountPossible(), 9u)
      << "With 9 as maximum value, we should have 9 possible values just after "
         "initialization.";
  EXPECT_EQ(s.Value(), 0u)
      << "Multiple possibilities should return 0 as a value.";
  for (unsigned i = 1u; i <= 9u; i++) {
    EXPECT_TRUE(s.IsPossible(i)) << "All values should be possible :" << i;
  }
  EXPECT_FALSE(s.IsSet());
  for (unsigned i = 0u; i < 9u; i++) {
    EXPECT_EQ(s.Next(i), i + 1u);
  }

  s = 3u;
  EXPECT_EQ(s.Value(), 3u) << "Manually set to 3";
  EXPECT_TRUE(s.IsPossible(3u));
  EXPECT_EQ(s.CountPossible(), 1u);
  EXPECT_TRUE(s.IsSet());
  EXPECT_EQ(s.Next(0u), 3u);
  EXPECT_EQ(s.Next(3u), 0u);

  s.Reset();
  EXPECT_EQ(s.CountPossible(), 9u)
      << "With 9 as maximum value, we should have 9 possible values just after "
         "initialization.";
  EXPECT_EQ(s.Value(), 0u)
      << "Multiple possibilities should return 0 as a value.";
  for (unsigned i = 1u; i <= 9u; i++) {
    EXPECT_TRUE(s.IsPossible(i)) << "All values should be possible :" << i;
  }
  EXPECT_FALSE(s.IsSet());
  for (unsigned i = 0u; i < 9u; i++) {
    EXPECT_EQ(s.Next(i), i + 1u);
  }
}
} // namespace