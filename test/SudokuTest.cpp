/* (c) 2020 RNDr. Simon Toth (happy.cerberus@gmail.com) */

#include "../src/Sudoku.h"
#include "../src/SmartSolver.h"
#include "../src/SolveStats.h"
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

TEST_CASE("Sudoku : X-Wing test 1", "") {
  Sudoku test(9);
  for (unsigned i = 0; i < 9; i++) {
    for (unsigned j = 0; j < 9; j++) {
      test[i][j] -= 4;
    }
  }

  test[6][3] += 4;
  test[6][4] += 4;
  test[6][6] += 4;
  test[1][4] += 4;
  test[1][6] += 4;


  test.SolveFish(2,4);
  unsigned count = 0;
  for (unsigned i = 0; i < 9; i++) {
    for (unsigned j = 0; j < 9; j++) {
      if (test[i][j].IsPossible(4)) count++;
    }
  }

  REQUIRE(!test[6][3].IsPossible(4));
  REQUIRE(count == 4);
}

TEST_CASE("Sudoku : X-Wing test 2", "") {
  Sudoku test(9);
  for (unsigned i = 0; i < 9; i++) {
    for (unsigned j = 0; j < 9; j++) {
      test[i][j] -= 4;
    }
  }
/*
[.23......] [........9] [.23......] [...456...] [1........] [....56...] [......7..] [.......8.] [...45....]
[1........] [.......8.] [.....6...] [.2.......] [...4..7..] [....5.7..] [...45....] [..3......] [........9]
[....5....] [...4.....] [......7..] [........9] [.......8.] [..3......] [.....6...] [.2.......] [1........]
[.23......] [1........] [....5....] [.......8.] [.....6...] [...4.....] [.23......] [........9] [......7..]
[.....6...] [.23...7..] [...4.....] [..3.5.7..] [.23...7.9] [....5.7.9] [.......8.] [1........] [.23.5....]
[........9] [.23...7..] [.......8.] [..3.5.7..] [.23...7..] [1........] [.23.5....] [...4.....] [.....6...]
[......7..] [.23......] [1........] [..34.6...] [..34....9] [.....6..9] [.234.....] [....5....] [.......8.]
[.......8.] [....5....] [........9] [..34..7..] [..3...7..] [.2.......] [1........] [.....6...] [..34.....]
[...4.....] [.....6...] [.23......] [1........] [....5....] [.......8.] [........9] [......7..] [.23......]
 */


  test[0][3] += 4;
  test[0][8] += 4;
  test[1][4] += 4;
  test[1][6] += 4;
  test[2][1] += 4;
  test[3][5] += 4;
  test[4][2] += 4;
  test[5][7] += 4;
  test[6][3] += 4;
  test[6][4] += 4;
  test[6][6] += 4;
  test[7][3] += 4;
  test[7][8] += 4;
  test[8][0] += 4;

  test.SolveFish(2,4);
  unsigned count = 0;
  for (unsigned i = 0; i < 9; i++) {
    for (unsigned j = 0; j < 9; j++) {
      if (test[i][j].IsPossible(4)) count++;
    }
  }

  REQUIRE(!test[6][3].IsPossible(4));
  REQUIRE(count == 13);
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
        INFO(i << j);
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

TEST_CASE("Sudoku : Solve test", "[debug]") {
  /*
  std::string puzzle = "000000700080200039507900620005060097600000810900001046700000050009002060460108070";
  Sudoku p(9);
  std::stringstream s(puzzle);
  s >> p;
  REQUIRE(p[0][6].IsSet());
  REQUIRE(p[0][6].Value() == 7);

  SolveStats stats;
  REQUIRE(SmartSolver::Solve(p, stats));

  std::string expect =
      "392615784186247539547983621215864397634729815978531246721396458859472163463158972";
  size_t offset = 0;
  for (unsigned i = 0; i < p.Size(); i++) {
    for (unsigned j = 0; j < p.Size(); j++) {
      REQUIRE(static_cast<unsigned>(expect[offset]-'0') == p[i][j].Value());
      offset++;
    }
  }
   */
}

} // namespace sudoku