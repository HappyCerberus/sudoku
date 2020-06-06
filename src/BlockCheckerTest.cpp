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
  EXPECT_FALSE(blockChecker.HasConflict());
  actual[0].Reset();
  EXPECT_FALSE(blockChecker.HasConflict());
  actual[0] = 3;
  EXPECT_TRUE(blockChecker.HasConflict());
}

TEST(BlockCheckerTest, Prune) {
  std::vector<Square> actual{Square(3u), Square(3u),
                                     Square(3u)};
  BlockChecker blockChecker(
      std::vector<Square *>{&actual[0], &actual[1], &actual[2]});
  blockChecker.Prune(1u);
  blockChecker.Prune(2u);
  for (unsigned i = 0; i < 3u; i++) {
    EXPECT_EQ(actual[i].Value(), 3u);
  }
  actual[0].Reset();
  blockChecker.Prune(1u);
  EXPECT_EQ(actual[0].CountPossible(), 2u);
}

TEST(BlockCheckerTest, recursive_set_find) {
  std::vector<Square> data(9, Square(9u));
  std::vector<Square *> squares;
  for (unsigned i = 0; i < static_cast<unsigned>(data.size()); i++) {
    squares.push_back(&data[i]);
  }

  {
    for (unsigned i = 0; i < static_cast<unsigned>(data.size()); i++) {
      data[i] = i + 1u;
    }
    std::vector<std::vector<unsigned>> result;
    recursive_set_find(result, squares, 1u);
    EXPECT_EQ(result.size(), 9u);
    EXPECT_THAT(result, testing::UnorderedElementsAre(
                            std::vector<unsigned>{0u},
                            std::vector<unsigned>{1u},
                            std::vector<unsigned>{2u},
                            std::vector<unsigned>{3u},
                            std::vector<unsigned>{4u},
                            std::vector<unsigned>{5u},
                            std::vector<unsigned>{6u},
                            std::vector<unsigned>{7u},
                            std::vector<unsigned>{8u}));
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
    std::vector<std::vector<unsigned>> result;
    recursive_set_find(result, squares, 2u);
    EXPECT_EQ(result.size(), 4lu);
    EXPECT_THAT(result,
                testing::UnorderedElementsAre(
                    std::vector<unsigned>{0u, 1u},
                    std::vector<unsigned>{3u, 4u},
                    std::vector<unsigned>{5u, 7u},
                    std::vector<unsigned>{6u, 8u}));
  }
}

TEST(BlockCheckerTest, recursive_number_find) {
  std::vector<Square> data(9, Square(9u));
  std::vector<Square *> squares;
  for (unsigned i = 0; i < static_cast<unsigned>(data.size()); i++) {
    squares.push_back(&data[i]);
  }

  {
    for (unsigned i = 0; i < static_cast<unsigned>(data.size()); i++) {
      data[i] = i + 1u;
    }
    std::vector<std::vector<unsigned>> result;
    recursive_number_find(result, squares, 1);
    EXPECT_EQ(result.size(), 9u);
    EXPECT_THAT(result, testing::UnorderedElementsAre(
                            std::vector<unsigned>{0u},
                            std::vector<unsigned>{1u},
                            std::vector<unsigned>{2u},
                            std::vector<unsigned>{3u},
                            std::vector<unsigned>{4u},
                            std::vector<unsigned>{5u},
                            std::vector<unsigned>{6u},
                            std::vector<unsigned>{7u},
                            std::vector<unsigned>{8u}));
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
    std::vector<std::vector<unsigned>> result;
    recursive_number_find(result, squares, 2);
    EXPECT_EQ(result.size(), 3u);
    EXPECT_THAT(result,
                testing::UnorderedElementsAre(
                    std::vector<unsigned>{0u, 1u},
                    std::vector<unsigned>{3u, 4u},
                    std::vector<unsigned>{7u, 8u}));
  }
}

}