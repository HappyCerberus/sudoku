/* (c) 2020 RNDr. Simon Toth (happy.cerberus@gmail.com) */

#include "../src/Sudoku.h"
#include <catch2/catch.hpp>
#include <sstream>

namespace sudoku {

std::vector<std::vector<std::vector<BlockChecker *>>> &
TestGetMappings(Sudoku &s) {
  return s.block_mapping_;
}

SudokuBlockType &TestGetBlockData(BlockChecker &s) { return s.elem_; }

TEST_CASE("Sudoku : Test Checkers", "") {
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

  REQUIRE(TestGetMappings(test3)[0][1].size() == 3);

  auto it = TestGetMappings(test3)[0][1].begin();
  for (unsigned j = 0; j < test3[0].Size(); j++) {
    REQUIRE(&test3[0][j] == TestGetBlockData(**it)[j]);
  }

  for (auto &i : TestGetMappings(test3)) {
    for (auto &j : i) {
      REQUIRE(j.size() == 3);
    }
  }
}
} // namespace sudoku