/* (c) 2020 RNDr. Simon Toth (happy.cerberus@gmail.com) */

#include "BlockChecker.h"
#include "Square.h"
#include "gmock/gmock-matchers.h"
#include "gtest/gtest.h"

namespace sudoku {

TEST(BlockCheckerTest, Check) {
  std::vector<Square> actual{Square(3u), Square(3u),
                                     Square(3u)};
  BlockChecker blockChecker(
      std::vector<Square *>{&actual[0], &actual[1], &actual[2]});
  actual[0] = 1;
  actual[1] = 2;
  actual[2] = 3;
  EXPECT_TRUE(blockChecker.Check());
  actual[0].Reset();
  EXPECT_TRUE(blockChecker.Check());
  actual[0] = 3;
  EXPECT_FALSE(blockChecker.Check());
}

TEST(BlockCheckerTest, Prune) {
  std::vector<Square> actual{Square(3u), Square(3u),
                                     Square(3u)};
  BlockChecker blockChecker(
      std::vector<Square *>{&actual[0], &actual[1], &actual[2]});
  blockChecker.Prune(1);
  blockChecker.Prune(2);
  for (size_t i = 0; i < 3; i++) {
    EXPECT_EQ(actual[i].Value(), 3);
  }
  actual[0].Reset();
  blockChecker.Prune(1);
  EXPECT_EQ(actual[0].CountPossible(), 2);
}

TEST(BlockCheckerTest, recursive_set_find) {
  std::vector<Square> data(9, Square(9u));
  std::vector<Square *> squares;
  for (size_t i = 0; i < data.size(); i++) {
    squares.push_back(&data[i]);
  }

  {
    for (size_t i = 0; i < data.size(); i++) {
      data[i] = i + 1u;
    }
    std::vector<std::vector<size_t>> result;
    recursive_set_find(result, squares, 1);
    EXPECT_EQ(result.size(), 9);
    EXPECT_THAT(result, testing::UnorderedElementsAre(
                            std::vector<size_t>{0}, std::vector<size_t>{1},
                            std::vector<size_t>{2}, std::vector<size_t>{3},
                            std::vector<size_t>{4}, std::vector<size_t>{5},
                            std::vector<size_t>{6}, std::vector<size_t>{7},
                            std::vector<size_t>{8}));
  }
  {
    data[0].OverrideValue(0b11u);
    data[1].OverrideValue(0b11u);
    data[2].OverrideValue(0b100u);
    data[3].OverrideValue(0b11000u);
    data[4].OverrideValue(0b11000u);
    data[5].OverrideValue(0b1100000u);
    data[6].OverrideValue(0b110000000u);
    data[7].OverrideValue(0b1100000u);
    data[8].OverrideValue(0b110000000u);
    std::vector<std::vector<size_t>> result;
    recursive_set_find(result, squares, 2);
    EXPECT_EQ(result.size(), 4);
    EXPECT_THAT(result,
                testing::UnorderedElementsAre(
                    std::vector<size_t>{0, 1}, std::vector<size_t>{3, 4},
                    std::vector<size_t>{5, 7}, std::vector<size_t>{6, 8}));
  }
}

TEST(BlockCheckerTest, recursive_number_find) {
  std::vector<Square> data(9, Square(9u));
  std::vector<Square *> squares;
  for (size_t i = 0; i < data.size(); i++) {
    squares.push_back(&data[i]);
  }

  {
    for (size_t i = 0; i < data.size(); i++) {
      data[i] = i + 1u;
    }
    std::vector<std::vector<size_t>> result;
    recursive_number_find(result, squares, 1);
    EXPECT_EQ(result.size(), 9);
    EXPECT_THAT(result, testing::UnorderedElementsAre(
                            std::vector<size_t>{0}, std::vector<size_t>{1},
                            std::vector<size_t>{2}, std::vector<size_t>{3},
                            std::vector<size_t>{4}, std::vector<size_t>{5},
                            std::vector<size_t>{6}, std::vector<size_t>{7},
                            std::vector<size_t>{8}));
  }
  {
    data[0].OverrideValue(0b111u);
    data[1].OverrideValue(0b111u);
    data[2].OverrideValue(0b100u);
    data[3].OverrideValue(0b11000u);
    data[4].OverrideValue(0b11000u);
    data[5].OverrideValue(0b1100000u);
    data[6].OverrideValue(0b111100000u);
    data[7].OverrideValue(0b1100000u);
    data[8].OverrideValue(0b111100000u);
    std::vector<std::vector<size_t>> result;
    recursive_number_find(result, squares, 2);
    EXPECT_EQ(result.size(), 3);
    EXPECT_THAT(result,
                testing::UnorderedElementsAre(std::vector<size_t>{0, 1},
                                              std::vector<size_t>{3, 4},
                                              std::vector<size_t>{7, 8}));
  }
}

}