/* (c) 2020 RNDr. Simon Toth (happy.cerberus@gmail.com) */

#include "Sudoku.h"
#include "gtest/gtest.h"

TEST(SudokuTest, SetupCheckers) {
  std::string small = "4  0  0   0  0  8   0  0  3 \n"
                      "0  0  5   2  0  0   0  1  0 \n"
                      "0  6  0   0  0  9   0  0  0 \n"
                      "\n"
                      "0  0  0   0  0  0   0  3  0 \n"
                      "0  0  6   9  0  1   0  0  0 \n"
                      "0  0  0   6  0  4   9  2  0 \n"
                      "\n"
                      "0  2  9   0  0  0   3  0  0 \n"
                      "0  0  4   0  0  2   0  8  5 \n"
                      "0  0  0   7  0  3   0  0  0 ";
  std::stringstream stream(small);
  Sudoku test3(9);
  stream >> test3;

  ASSERT_EQ(test3.block_mapping_[0][1].size(), 3);

  auto it = test3.block_mapping_[0][1].begin();
  for (size_t j = 0; j < test3.data_[0].size(); j++) {
    EXPECT_EQ(&test3.data_[0][j], (*it)->elem_[j]);
  }

  for (auto &i : test3.block_mapping_) {
    for (auto &j : i) {
      EXPECT_EQ(j.size(), 3) << "For a normal Sudoku we expect every square "
                                "to belong to 3 blocks.";
    }
  }
}