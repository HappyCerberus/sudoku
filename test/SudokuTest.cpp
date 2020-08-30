/* (c) 2020 RNDr. Simon Toth (happy.cerberus@gmail.com) */

#include "../src/Sudoku.h"
#include "../src/SmartSolver.h"
#include "../src/SolveStats.h"
#include <catch2/catch.hpp>
#include <sstream>
#include <iostream>

namespace sudoku {

std::vector<std::vector<std::vector<UniqueBlock *>>> &
TestGetMappings(Sudoku &s) {
  return s.block_mapping_;
}

void TestInjectKillerBlock(Sudoku &s, unsigned sum, const std::vector<unsigned>& offsets) {
    std::vector<BitSet*> squares;
    for (unsigned o : offsets) {
        squares.push_back(&s.data_[o]);
    }
    s.killers_.emplace_back(squares, s.Max(), sum);
}


void ClearSudoku(Sudoku &s) {
  for (unsigned i = 0; i < s.Size(); i++) {
    for (unsigned j = 0; j < s.Size(); j++) {
      s[i][j] = BitSet::Empty(s.Size());
    }
  }
}

std::vector<::sudoku::BitSet *> &TestGetBlockData(UniqueBlock &s) { return s.elem_; }

TEST_CASE("Sudoku : simple operations", "[basic]") {
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

  REQUIRE(test[0][0].SingletonValue() == 4);
  REQUIRE(test[0][8].SingletonValue() == 3);
  REQUIRE(test[7][7].SingletonValue() == 8);

  REQUIRE(test.Size() == 9u);

  std::string solution = "931625478 568734291 724198356 483572619 159486723 276913845 897341562 612857934 345269187";
  stream.str(solution);
  stream >> test;
  REQUIRE(test.IsSet());
  test[3][4] += 9;
  REQUIRE(!test.IsSet());
}

TEST_CASE("Sudoku : change tracking", "[change]") {
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
  CHECK(!test.HasChange());

  stream >> test;
  CHECK(test.HasChange());
  test.ResetChange();

  test[8][0] = BitSet::SingleBit(9u, 1u);
  CHECK(test.HasChange());

  std::unordered_set<UniqueBlock *> blocks = test.ChangedBlocks();
  CHECK(blocks.size() == 3);

  test[7][1] = BitSet::SingleBit(9u, 2u);
  blocks = test.ChangedBlocks();
  CHECK(blocks.size() == 5);
}

TEST_CASE("Sudoku : check against solution", "[solution]") {
  std::string small = R"(
    0 3 1 0 0 5 4 0 0
    0 0 8 0 0 0 0 0 1
    7 0 0 0 9 0 0 5 6
    4 0 0 5 0 2 0 0 0
    0 0 9 0 0 0 7 0 0
    0 0 0 9 0 3 0 0 5
    8 9 0 0 4 0 0 0 2
    6 0 0 0 0 0 9 0 0
    0 0 5 2 0 0 1 8 0 
  )";
  std::string solution = "931625478 568734291 724198356 483572619 159486723 276913845 897341562 612857934 345269187";
  std::stringstream stream(small);
  Sudoku test(9u);
  stream >> test;

  Sudoku solved(9u);
  stream.str(solution);
  stream >> solved;
  test.SetSolution(&solved);
  REQUIRE(test.CheckAgainstSolution());

  test[0][0] = BitSet::SingleBit(9u, 7u);
  REQUIRE(!test.CheckAgainstSolution());

  test[0][0] = BitSet::SudokuSquare(9u);
  test[0][3] -= 6;
  REQUIRE(!test.CheckAgainstSolution());
}


TEST_CASE("Sudoku : Test Build Chains", "[graph]") {
  Sudoku test(9);
  for (unsigned i = 0; i < 9; i++) {
    for (unsigned j = 0; j < 9; j++) {
      test[i][j] -= 1;
    }
  }

  // https://happycerberus.github.io/sudoku_playground/?puzzle=0:9:9:9:511:511:511:511:1:511:511:511:511:511:1:511:511:511:511:511:1:511:511:511:511:511:511:511:511:511:511:511:511:511:511:511:1:511:511:511:511:511:511:511:511:511:511:511:511:511:1:511:511:511:511:511:1:1:511:511:511:511:511:511:511:511:511:511:1:511:511:511:511:511:1:511:511:511:511:511:511:511:511:511:1:1:
  test[0][4] += 1; // abs = 4
  test[1][1] += 1; // abs = 10
  test[1][7] += 1; // abs = 16
  test[3][5] += 1; // abs = 32
  test[5][1] += 1; // abs = 46
  test[5][7] += 1; // abs = 52
  test[5][8] += 1; // abs = 53
  test[7][1] += 1; // abs = 64
  test[7][7] += 1; // abs = 70
  test[8][8] += 1; // abs = 80

  auto chains = test.GetChains(1u);
  CHECK(chains.nodes.size() == 10);

  CHECK(chains.weak_links.count(4) == 0);
  CHECK(chains.strong_links.count(4) == 0);
  CHECK(chains.weak_links.count(32) == 0);
  CHECK(chains.strong_links.count(32) == 0);

  CHECK(find(chains.nodes.begin(), chains.nodes.end(), 10) != chains.nodes.end());
  CHECK(chains.strong_links.count(10) == 1);
  CHECK(chains.weak_links.count(10) == 3);

  CHECK(find(chains.nodes.begin(), chains.nodes.end(), 80) != chains.nodes.end());
  CHECK(chains.strong_links.count(80) == 2);
  CHECK(chains.weak_links.count(80) == 2);
}

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

  std::vector<unsigned> result;
  for (unsigned i = 0; i < 9; i++) {
    for (unsigned j = 0; j < 9; j++) {
      result.push_back(i*9+j);
    }
  }

  for (unsigned j = 0; j < 9; j++) {
    for (unsigned i = 0; i < 9; i++) {
      result.push_back(i*9+j);
    }
  }

  for (unsigned i = 0; i < 3; i++) {
    for (unsigned j = 0; j < 3; j++) {
      for (unsigned x = i*3; x < (i+1)*3; x++) {
        for (unsigned y = j*3; y < (j+1)*3; y++) {
          result.push_back(x*9+y);
        }
      }
    }
  }

  unsigned it = 0;
  for (const auto& block : test3.Blocks()) {
    for (auto *square : block.GetSquares()) {
      CHECK(square-&test3[0][0] == result[it]);
      it++;
    }
  }

  it = 0;
  for (const auto& block : test3.GetRowBlocks()) {
    for (auto *square : block->GetSquares()) {
      CHECK(square-&test3[0][0] == result[it]);
      it++;
    }
  }

  for (const auto& block : test3.GetColBlocks()) {
    for (auto *square : block->GetSquares()) {
      CHECK(square-&test3[0][0] == result[it]);
      it++;
    }
  }

  for (auto &i : TestGetMappings(test3)) {
    for (auto &j : i) {
      REQUIRE(j.size() == 3);
    }
  }

  it = 0;
  for (auto &i : TestGetMappings(test3)[0][0][0]->GetSquares()) {
    CHECK(i-&test3[0][0] == it);
    it++;
  }
  it = 0;
  for (auto &i : TestGetMappings(test3)[0][0][1]->GetSquares()) {
    CHECK(i-&test3[0][0] == it*9);
    it++;
  }
  it = 0;
  for (auto &i : TestGetMappings(test3)[0][0][2]->GetSquares()) {
    CHECK(i-&test3[0][0] == ((it/3)*9+it%3));
    it++;
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

TEST_CASE("Sudoku : Changed blocks", "[]") {
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

  auto blocks = test3.ChangedBlocks();
  CHECK(blocks.size() == 26);

  for (auto b : test3.GetRowBlocks()) {
    CHECK(blocks.count(b) == 1);
  }
  auto cols = test3.GetColBlocks();
  for (unsigned i = 0; i < cols.size(); i++) {
    if (i != 4)    
      CHECK(blocks.count(cols[i]) == 1);
    else
      CHECK(blocks.count(cols[i]) == 0);
  }
}

TEST_CASE("Sudoku : Solve Chain Test A", "[]") {
  // http://hodoku.sourceforge.net/en/tech_chains.php
  // X-chain first example
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
      test[i][j] = BitSet::Empty(9u);
      expect[i][j] = BitSet::Empty(9u);
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
    //REQUIRE(test.Serialize() == "0:9:9:9:8:256:2:16:1:128:64:32:4:128:4:16:2:32:64:8:1:256:64:32:1:4:8:256:128:16:2:256:8:64:128:2:16:1:4:32:2:128:32:256:4:1:16:8:64:1:16:4:32:64:8:256:2:128:16:2:256:8:128:32:4:64:1:4:64:8:1:256:2:32:128:16:32:1:128:64:16:4:2:256:8:1:");
}

TEST_CASE("Sudoku : Serialize and Deserialize : Killer", "[killer]") {
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

    TestInjectKillerBlock(test, 15, {1, 2, 11});
    TestInjectKillerBlock(test, 9, {6,7});

    std::string data = test.Serialize();
    REQUIRE(data == "0:9:9:9:8:511:511:511:511:128:511:511:4:511:511:16:2:511:511:511:1:511:511:32:511:511:511:256:511:511:511:511:511:511:511:511:511:511:4:511:511:511:32:256:511:1:511:511:511:511:511:511:32:511:8:256:2:511:511:2:256:511:511:511:4:511:511:511:511:8:511:511:2:511:128:16:511:511:511:64:511:4:511:511:511:1:0:3:15:1:2:11:0:2:9:6:7:");

    Sudoku test2(9);
    try {
        test2.Deserialize(data);
    } catch(std::exception& e) {
        INFO(e.what());
        CHECK(false);
    }

    // check that the killer blocks match
    for (unsigned i = 0; i < 9; i++) {
        for (unsigned j = 0; j < 9; j++) {
            REQUIRE(test[i][j] == test2[i][j]);
        }
    }
    REQUIRE(test.Serialize() == test2.Serialize());

    test.PreBuildKillerMapping();

    auto &mapping = TestGetContainedKillerBlocks(test);
    CHECK(mapping.size() == 3);
    CHECK(mapping.count(0) == 1);
    CHECK(mapping.count(18) == 1);
    CHECK(mapping.count(20) == 1);
    CHECK((*mapping.equal_range(0).first).second == 1);
    CHECK((*mapping.equal_range(18).first).second == 0);
    CHECK((*mapping.equal_range(20).first).second == 1);

    SolveStats s;
    SmartSolver::Solve(test,s);
    SmartSolver::Solve(test2,s);
    for (unsigned i = 0; i < 9; i++) {
        for (unsigned j = 0; j < 9; j++) {
            REQUIRE(test[i][j] == test2[i][j]);
        }
    }
    REQUIRE(test.Serialize() == test2.Serialize());
    //REQUIRE(test.Serialize() == "0:9:9:9:8:256:2:16:1:128:64:32:4:128:4:16:2:32:64:8:1:256:64:32:1:4:8:256:128:16:2:256:8:64:128:2:16:1:4:32:2:128:32:256:4:1:16:8:64:1:16:4:32:64:8:256:2:128:16:2:256:8:128:32:4:64:1:4:64:8:1:256:2:32:128:16:32:1:128:64:16:4:2:256:8:1:");
}

TEST_CASE("Sudoku : SolveFinnedFish 1", "[finned]") {
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

    test.SolveFinnedFish(2,1);

    for (unsigned i = 0; i < 9; i++) {
      for (unsigned j = 0; j < 9; j++) {
        INFO(i << ":" << j);
        CHECK(test[i][j] == expect[i][j]);
      }
    }
  }

TEST_CASE("Sudoku : SolveFinnedFish 2", "[finned]") {
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

  test.SolveFinnedFish(2,1);

  for (unsigned i = 0; i < 9; i++) {
    for (unsigned j = 0; j < 9; j++) {
      INFO(i << ":" << j);
      CHECK(test[i][j] == expect[i][j]);
    }
  }
}

TEST_CASE("Sudoku : SolveFinnedFish 3", "[finned]") {
  Sudoku test(9);
  Sudoku expect(9);
    
  for (unsigned j = 0; j < 9; j++) {
    if (j == 2 || j == 4 || j == 6 )
      continue;
    test[j][2] -= 1;
    test[j][5] -= 1;
    expect[j][2] -= 1;
    expect[j][5] -= 1;
  }

  test.SolveFinnedFish(2, 1);

  for (unsigned i = 0; i < 9; i++) {
    for (unsigned j = 0; j < 9; j++) {
      INFO(i << ":" << j);
      CHECK(test[i][j] == expect[i][j]);
    }
  }
}

TEST_CASE("Sudoku : SolveFinnedFish 4", "[finned]") {
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

  test[3][5] += 1;
  expect[3][5] += 1;

  test.SolveFish(2,1);

  for (unsigned i = 0; i < 9; i++) {
    for (unsigned j = 0; j < 9; j++) {
      INFO(i << ":" << j);
      CHECK(test[i][j] == expect[i][j]);
    }
  }
}

TEST_CASE("Sudoku : SolveFinnedFish 5", "[finned]") {
  Sudoku test(9);
  Sudoku expect(9);
    
  for (unsigned j = 0; j < 9; j++) {
    if (j == 2 || j == 6)
      continue;
    test[3][j] -= 1;
    test[5][j] -= 1;
    expect[3][j] -= 1;
    expect[5][j] -= 1;
  }

  test[3][0] += 1;
  expect[3][0] += 1;
  expect[4][2] -= 1;

  test.SolveFinnedFish(2,1);

  for (unsigned i = 0; i < 9; i++) {
    for (unsigned j = 0; j < 9; j++) {
      INFO(i << ":" << j);
      CHECK(test[i][j] == expect[i][j]);
    }
  }
}

/*
TEST_CASE("Sudoku : Killer sudoku", "[debug]") {
    std::vector<KillerCage> cages;
    // https://www.dailykillersudoku.com/puzzle/20510
    cages.emplace_back(KillerCage{.sum = 21, .coords = {{0,0}, {0, 1}, {1, 0}}});
    cages.emplace_back(KillerCage{.sum = 6, .coords = {{0, 2}, {1, 2}}});
    cages.emplace_back(KillerCage{.sum = 10, .coords = {{0, 3}, {1, 3}}});
    cages.emplace_back(KillerCage{.sum = 10, .coords = {{0, 4}, {1, 4}}});
    cages.emplace_back(KillerCage{.sum = 13, .coords = {{0, 5}, {1, 5}}});
    cages.emplace_back(KillerCage{.sum = 12, .coords = {{0, 6}, {1, 6}}});
    cages.emplace_back(KillerCage{.sum = 11, .coords = {{0, 7}, {0, 8}, {1, 8}}});
    cages.emplace_back(KillerCage{.sum = 9, .coords = {{1, 1}, {2, 1}}});
    cages.emplace_back(KillerCage{.sum = 18, .coords = {{1, 7}, {2, 7}, {2, 8}}});
    cages.emplace_back(KillerCage{.sum = 18, .coords = {{2, 0}, {3, 0}, {3, 1}}});
    cages.emplace_back(KillerCage{.sum = 19, .coords = {{2, 2}, {3, 2}, {4, 2}}});
    cages.emplace_back(KillerCage{.sum = 9, .coords = {{2, 3}, {3, 3}}});
    cages.emplace_back(KillerCage{.sum = 9, .coords = {{2, 4}, {3, 4}}});
    cages.emplace_back(KillerCage{.sum = 15, .coords = {{2, 5}, {3, 5}, {2, 6}}});
    cages.emplace_back(KillerCage{.sum = 9, .coords = {{3, 6}, {3, 7}, {4, 7}}});
    cages.emplace_back(KillerCage{.sum = 6, .coords = {{3, 8}, {4, 8}}});
    cages.emplace_back(KillerCage{.sum = 3, .coords = {{4, 0}, {5, 0}}});
    cages.emplace_back(KillerCage{.sum = 14, .coords = {{4, 1}, {5, 1}, {5, 2}}});
    cages.emplace_back(KillerCage{.sum = 19, .coords = {{4, 3}, {4, 4}, {4, 5}}});
    cages.emplace_back(KillerCage{.sum = 21, .coords = {{4, 6}, {5, 6}, {6, 6}}});
    cages.emplace_back(KillerCage{.sum = 15, .coords = {{5, 3}, {6, 2}, {6, 3}}});
    cages.emplace_back(KillerCage{.sum = 8, .coords = {{5, 4}, {6, 4}}});
    cages.emplace_back(KillerCage{.sum = 7, .coords = {{5, 5}, {6, 5}}});
    cages.emplace_back(KillerCage{.sum = 20, .coords = {{5, 7}, {5, 8}, {6, 8}}});
    cages.emplace_back(KillerCage{.sum = 14, .coords = {{6, 0}, {6, 1}, {7, 1}}});
    cages.emplace_back(KillerCage{.sum = 13, .coords = {{6, 7}, {7, 7}}});
    cages.emplace_back(KillerCage{.sum = 18, .coords = {{7, 0}, {8, 0}, {8, 1}}});
    cages.emplace_back(KillerCage{.sum = 12, .coords = {{7, 2}, {8, 2}}});
    cages.emplace_back(KillerCage{.sum = 6, .coords = {{7, 3}, {8, 3}}});
    cages.emplace_back(KillerCage{.sum = 9, .coords = {{7, 4}, {8, 4}}});
    cages.emplace_back(KillerCage{.sum = 10, .coords = {{7, 5}, {8, 5}}});
    cages.emplace_back(KillerCage{.sum = 7, .coords = {{7, 6}, {8, 6}}});
    cages.emplace_back(KillerCage{.sum = 14, .coords = {{7, 8}, {8, 7}, {8, 8}}});

    Sudoku test(9, BASIC, cages);
}
*/
} // namespace sudoku