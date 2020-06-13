/* (c) 2020 RNDr. Simon Toth (happy.cerberus@gmail.com) */

#include "../src/Sudoku.h"
#include <catch2/catch.hpp>
#include <sstream>
#include <iostream>

namespace sudoku {

std::vector<std::vector<std::vector<BlockChecker *>>> &
TestGetMappings(Sudoku &s) {
  return s.block_mapping_;
}


void ClearSudoku(Sudoku &s) {
  for (unsigned i = 0; i < s.Size(); i++) {
    for (unsigned j = 0; j < s.Size(); j++) {
      s[i][j].ResetToEmpty();
    }
  }
}

SudokuBlockType &TestGetBlockData(BlockChecker &s) { return s.elem_; }

TEST_CASE("Sudoku : Test Checkers", "x") {
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

TEST_CASE("Sudoku : Fish", "") {
  Sudoku test(9);
  for (unsigned j = 0; j < 9; j++) {
    if (j == 2 || j == 6) continue;
    test[2][j] -= 1;
    test[5][j] -= 1;
  }

  test.SolveFish(2,1);
  for (unsigned i = 0; i < 9; i++) {
    for (unsigned j = 0; j < 9; j++) {
      if (i == 2 || i == 5) {
        REQUIRE(test[i][j].IsPossible(1) == (j == 2 || j == 6));
      } else {
        REQUIRE(test[i][j].IsPossible(1) != (j == 2 || j == 6));
      }
    }
  }
}

TEST_CASE("Sudoku : Finned Fish", "") {
  {
    Sudoku test(9);
    Sudoku expect(9);
    for (unsigned j = 0; j < 9; j++) {
      if (j == 2 || j == 6)
        continue;
      test[2][j] -= 1;
      test[5][j] -= 1;
      expect[2][j] -= 1;
      expect[5][j] -= 1;
    }

    test[5][0] += 1;
    expect[5][0] += 1;
    expect[3][2] -= 1;
    expect[4][2] -= 1;

    test.SolveFinnedFish(2, 1);

    for (unsigned i = 0; i < 9; i++) {
      for (unsigned j = 0; j < 9; j++) {
        REQUIRE(test[i][j] == expect[i][j]);
      }
    }
  }

  {
    Sudoku test(9);
    Sudoku expect(9);
    for (unsigned j = 0; j < 9; j++) {
      if (j == 2 || j == 6)
        continue;
      test[j][2] -= 1;
      test[j][5] -= 1;
      expect[j][2] -= 1;
      expect[j][5] -= 1;
    }

    test[0][5] += 1;
    expect[0][5] += 1;
    expect[2][3] -= 1;
    expect[2][4] -= 1;

    test.SolveFinnedFish(2, 1);

    for (unsigned i = 0; i < 9; i++) {
      for (unsigned j = 0; j < 9; j++) {
        REQUIRE(test[i][j] == expect[i][j]);
      }
    }
  }

}

} // namespace sudoku