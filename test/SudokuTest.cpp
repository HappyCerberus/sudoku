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

std::vector<::sudoku::Square *> &TestGetBlockData(BlockChecker &s) { return s.elem_; }

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


TEST_CASE("Sudoku : Finned Fish Debug", "[debug]") {
  Sudoku test(9);
  Sudoku expect(9);
  for (unsigned i = 0; i < 9; i++) {
    for (unsigned j = 0; j < 9; j++) {
      test[i][j] -= 8;
      expect[i][j] -= 8;
    }
  }

  test[0][4] += 8;
  test[1][7] += 8;
  test[1][8] += 8;
  test[2][0] += 8;
  test[3][1] += 8;
  test[3][3] += 8;
  test[3][7] += 8;
  test[3][8] += 8;
  test[4][1] += 8;
  test[4][5] += 8;
  test[4][8] += 8;
  test[5][1] += 8;
  test[5][7] += 8;
  test[6][3] += 8;
  test[6][5] += 8;
  test[7][2] += 8;
  test[8][6] += 8;


  expect[0][4] += 8;
  expect[1][7] += 8;
  expect[1][8] += 8;
  expect[2][0] += 8;
  expect[3][1] += 8;
  expect[3][3] += 8;
  expect[3][7] += 8;
  expect[3][8] += 8;
  expect[4][1] += 8;
  expect[4][5] += 8;
  expect[4][8] += 8;
  expect[5][1] += 8;
  expect[5][7] += 8;
  expect[6][3] += 8;
  expect[6][5] += 8;
  expect[7][2] += 8;
  expect[8][6] += 8;

  /*
  [........9] [..3......] [...4..7..] [...4..7..] [.......8.] [....5....] [.....6...] [.2.......] [1........]
  [.....6...] [.2.......] [....5....] [..3......] [1........] [........9] [......7..] [...4...8.] [...4...8.]
  [.......8.] [1........] [1..4..7..] [...4.67..] [...4.6...] [.2.......] [..3.....9] [..3.....9] [....5....]
  [....5....] [.....6.8.] [.2.......] [.....6.89] [......7..] [...4.....] [1........] [..3....89] [..3..6...]
  [..3......] [1..4.6.8.] [1..4....9] [.2.......] [....5....] [.....6.8.] [...4....9] [......7..] [...4.6.8.]
  [......7..] [...4.6.8.] [...4....9] [1........] [.....6..9] [..3......] [...45....] [...45..89] [.2.......]
  [1........] [....5.7..] [.....6...] [...4...8.] [..3......] [......78.] [.2.......] [...45....] [........9]
  [.2.......] [....5.7..] [.......8.] [...4.6..9] [...4.6..9] [.....67..] [..345....] [1........] [..34.....]
  [...4.....] [........9] [..3......] [....5....] [.2.......] [1........] [.......8.] [.....6...] [......7..]
*/


  test.SolveFinnedFish(4, 8);

  for (unsigned i = 0; i < 9; i++) {
    for (unsigned j = 0; j < 9; j++) {
      INFO(i << j);
      CHECK(test[i][j] == expect[i][j]);
    }
  }
}

TEST_CASE("Sudoku : Prune from chain", "[]") {
  Sudoku test(9);
  Sudoku expect(9);
  for (unsigned i = 0; i < 9; i++) {
    for (unsigned j = 0; j < 9; j++) {
      test[i][j] -= 8;
      expect[i][j] -= 8;
    }
  }

  for (unsigned i = 0; i < 9; i++) {
    test[i][1] += 8;
  }
  test[1][6] += 8;

  expect[1][6] += 8;
  expect[1][1] += 8;
  expect[5][1] += 8;
  expect[8][1] += 8;

  std::vector<unsigned> path;
  path.push_back(8*9+1);
  path.push_back(1*9+6);
  path.push_back(5*9+1);
  path.push_back(1*9+1);

  test.PruneNumbersSeenFrom(path, 8);
  for (unsigned i = 0; i < 9; i++) {
    for (unsigned j = 0; j < 9; j++) {
      INFO(i << j);
      CHECK(test[i][j] == expect[i][j]);
    }
  }
}

TEST_CASE("Sudoku : Solve Chain Test A", "[]") {
  Sudoku test(9);
  Sudoku expect(9);
  for (unsigned i = 0; i < 9; i++) {
    for (unsigned j = 0; j < 9; j++) {
      test[i][j] -= 7;
      expect[i][j] -= 7;
    }
  }

  test[0][1] += 7;
  test[0][8] += 7;
  test[1][0] += 7;
  test[1][1] += 7;
  test[1][7] += 7;
  test[1][8] += 7;
  test[2][4] += 7;
  test[3][0] += 7;
  test[3][1] += 7;
  test[3][2] += 7;
  test[4][3] += 7;
  test[5][6] += 7;
  test[6][0] += 7;
  test[6][2] += 7;
  test[6][7] += 7;
  test[6][8] += 7;
  test[7][0] += 7;
  test[7][1] += 7;
  test[7][8] += 7;
  test[8][5] += 7;

  expect[0][1] += 7;
  expect[0][8] += 7;
  expect[1][0] += 7;
  expect[1][1] += 7;
  expect[1][7] += 7;
  expect[1][8] += 7;
  expect[2][4] += 7;
  expect[3][0] += 7;
  expect[3][2] += 7;
  expect[4][3] += 7;
  expect[5][6] += 7;
  expect[6][0] += 7;
  expect[6][2] += 7;
  expect[6][7] += 7;
  expect[6][8] += 7;
  expect[7][0] += 7;
  expect[7][1] += 7;
  expect[7][8] += 7;
  expect[8][5] += 7;

  test.SolveXChains(6, 7);
  for (unsigned i = 0; i < 9; i++) {
    for (unsigned j = 0; j < 9; j++) {
      INFO(i << j);
      CHECK(test[i][j] == expect[i][j]);
    }
  }
}

TEST_CASE("Sudoku : Solve Chain Test B", "[]") {
  Sudoku test(9);
  Sudoku expect(9);
  for (unsigned i = 0; i < 9; i++) {
    for (unsigned j = 0; j < 9; j++) {
      test[i][j] -= 3;
      expect[i][j] -= 3;
    }
  }

  test[0][3] += 3;
  test[1][6] += 3;
  test[2][1] += 3;
  test[2][2] += 3;
  test[3][1] += 3;
  test[3][7] += 3;
  test[4][5] += 3;
  test[4][7] += 3;
  test[4][8] += 3;
  test[5][0] += 3;
  test[5][1] += 3;
  test[5][2] += 3;
  test[5][4] += 3;
  test[5][8] += 3;
  test[6][4] += 3;
  test[6][5] += 3;
  test[6][8] += 3;
  test[7][0] += 3;
  test[7][1] += 3;
  test[7][2] += 3;
  test[7][5] += 3;
  test[7][7] += 3;
  test[8][0] += 3;
  test[8][2] += 3;
  test[8][5] += 3;

  expect[0][3] += 3;
  expect[1][6] += 3;
  expect[2][1] += 3;
  expect[2][2] += 3;
  expect[3][1] += 3;
  expect[3][7] += 3;
  expect[4][5] += 3;
  expect[4][7] += 3;
  expect[4][8] += 3;
  expect[5][4] += 3;
  expect[5][8] += 3;
  expect[6][4] += 3;
  expect[6][5] += 3;
  expect[6][8] += 3;
  expect[7][0] += 3;
  expect[7][1] += 3;
  expect[7][2] += 3;
  expect[7][5] += 3;
  expect[7][7] += 3;
  expect[8][0] += 3;
  expect[8][2] += 3;
  expect[8][5] += 3;

  test.SolveXChains(6, 3);
  for (unsigned i = 0; i < 9; i++) {
    for (unsigned j = 0; j < 9; j++) {
      INFO(i << j);
      CHECK(test[i][j] == expect[i][j]);
    }
  }
}


TEST_CASE("Sudoku : Solve XY Chain Test A", "[]") {
  Sudoku test(9);
  Sudoku expect(9);
  for (unsigned i = 0; i < 9; i++) {
    for (unsigned j = 0; j < 9; j++) {
      test[i][j].ResetToEmpty();
      expect[i][j].ResetToEmpty();
    }
  }

  test[1][3] += 3;
  test[1][4] += 2;
  test[1][5] += 2;
  test[1][5] += 3;
  test[1][8] += 3;
  test[2][3] += 3;
  test[2][5] += 8;
  test[2][7] += 3;
  test[4][3] += 8;
  test[4][3] += 9;
  test[4][4] += 9;
  test[4][5] += 2;
  test[4][5] += 8;
  test[6][3] += 3;
  test[6][3] += 9;
  test[6][4] += 9;
  test[6][5] += 3;
  test[7][7] += 3;
  test[7][8] += 3;


  expect[1][4] += 2;
  expect[1][5] += 2;
  expect[1][5] += 3;
  expect[1][8] += 3;
  expect[2][5] += 8;
  expect[2][7] += 3;
  expect[4][3] += 8;
  expect[4][3] += 9;
  expect[4][4] += 9;
  expect[4][5] += 2;
  expect[4][5] += 8;
  expect[6][3] += 3;
  expect[6][3] += 9;
  expect[6][4] += 9;
  expect[7][7] += 3;
  expect[7][8] += 3;

  test.SolveXYChains();
  for (unsigned i = 0; i < 9; i++) {
    for (unsigned j = 0; j < 9; j++) {
      INFO(i << j);
      CHECK(test[i][j] == expect[i][j]);
    }
  }
}

TEST_CASE("Sudoku : Serialize and Deserialize", "x") {
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
    Sudoku test(9);
    stream >> test;

    std::string data = test.Serialize();
    REQUIRE(data == "0:9:9:9:8:511:511:511:511:128:511:511:4:511:511:16:2:511:511:511:1:511:511:32:511:511:511:256:511:511:511:511:511:511:511:511:511:511:4:511:511:511:32:256:511:1:511:511:511:511:511:511:32:511:8:256:2:511:511:2:256:511:511:511:4:511:511:511:511:8:511:511:2:511:128:16:511:511:511:64:511:4:511:511:511:1:");

    Sudoku test2(9);
    try {
        test2.Deserialize(data);
    } catch(std::exception& e) {
        INFO(e.what());
        CHECK(false);
    }

    for (unsigned i = 0; i < 9; i++) {
        for (unsigned j = 0; j < 9; j++) {
            REQUIRE(test[i][j] == test2[i][j]);
        }
    }
    REQUIRE(test.Serialize() == test2.Serialize());

    SolveStats s;
    SmartSolver::Solve(test,s);
    SmartSolver::Solve(test2,s);
    for (unsigned i = 0; i < 9; i++) {
        for (unsigned j = 0; j < 9; j++) {
            REQUIRE(test[i][j] == test2[i][j]);
        }
    }
    REQUIRE(test.Serialize() == test2.Serialize());
    REQUIRE(test.Serialize() == "0:9:9:9:8:256:2:16:1:128:64:32:4:128:4:16:2:32:64:8:1:256:64:32:1:4:8:256:128:16:2:256:8:64:128:2:16:1:4:32:2:128:32:256:4:1:16:8:64:1:16:4:32:64:8:256:2:128:16:2:256:8:128:32:4:64:1:4:64:8:1:256:2:32:128:16:32:1:128:64:16:4:2:256:8:1:");
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