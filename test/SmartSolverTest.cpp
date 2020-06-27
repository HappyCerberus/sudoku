#include "../src/SmartSolver.h"
#include "../src/SolveStats.h"
#include <catch2/catch.hpp>
#include <sstream>

namespace sudoku {

TEST_CASE("Solver : Test Solve", "x") {
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

  SolveStats stats;
  REQUIRE(SmartSolver::Solve(test3, stats));
  REQUIRE(stats.groups[1] != 0);
}

TEST_CASE("Solver : SingleStep", "[]") {
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

  std::vector<sudoku::Square> state(9*9, sudoku::Square(9u));

  SolveStats stats;
  SmartSolver::SolveGroups(1, test, stats, state);
  CHECK(test[0][1].IsPossible(1));
  CHECK(!test[0][1].IsPossible(2));
  CHECK(!test[0][1].IsPossible(3));
  CHECK(!test[0][1].IsPossible(4));
  CHECK(!test[0][1].IsPossible(5));
  CHECK(!test[0][1].IsPossible(6));
  CHECK(test[0][1].IsPossible(7));
  CHECK(!test[0][1].IsPossible(8));
  CHECK(test[0][1].IsPossible(9));

  CHECK(test[8][4].IsPossible(1));
  CHECK(!test[8][4].IsPossible(2));
  CHECK(!test[8][4].IsPossible(3));
  CHECK(test[8][4].IsPossible(4));
  CHECK(test[8][4].IsPossible(5));
  CHECK(test[8][4].IsPossible(6));
  CHECK(!test[8][4].IsPossible(7));
  CHECK(test[8][4].IsPossible(8));
  CHECK(test[8][4].IsPossible(9));

}

} // namespace sudoku