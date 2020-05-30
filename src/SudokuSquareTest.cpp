// (c) 2020 RNDr. Simon Toth (happy.cerberus@gmail.com)

#include "SudokuSquare.h"
#include "gtest/gtest.h"

/*
bool IsPossible(unsigned number)
unsigned CountPossible()
bool IsSet() const
unsigned Value() const
unsigned Next(unsigned number)
void Reset()
void Set(unsigned number)
void Remove(unsigned number)
*/
namespace {
TEST(SudokuSquareTest, Empty) {
  SudokuSquare<uint16_t> s(9);
  EXPECT_EQ(s.CountPossible(), 9)
      << "With 9 as maximum value, we should have 9 possible values just after "
         "initialization.";
  EXPECT_EQ(s.Value(), 0)
      << "Multiple possibilities should return 0 as a value.";
  for (int i = 1; i <= 9; i++) {
    EXPECT_TRUE(s.IsPossible(i)) << "All values should be possible :" << i;
  }
  EXPECT_FALSE(s.IsSet());
  for (int i = 0; i < 9; i++) {
    EXPECT_EQ(s.Next(i), i + 1);
  }

  s.Set(3);
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
  for (int i = 1; i <= 9; i++) {
    EXPECT_TRUE(s.IsPossible(i)) << "All values should be possible :" << i;
  }
  EXPECT_FALSE(s.IsSet());
  for (int i = 0; i < 9; i++) {
    EXPECT_EQ(s.Next(i), i + 1);
  }
}
} // namespace