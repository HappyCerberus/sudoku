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
}

TEST_CASE("Solver : Puzzle Solve Test", "x") {
    std::string small = "0  2  0   0  0  9   0  5  0 \n"
                        "0  0  4   0  7  0   2  0  0 \n"
                        "0  5  0   4  0  6   0  0  0 \n"
                        "\n"
                        "1  0  6   0  0  7   0  0  0 \n"
                        "0  0  8   0  9  0   1  0  0 \n"
                        "0  0  0   3  0  0   4  0  7 \n"
                        "\n"
                        "0  0  0   9  0  2   0  6  0 \n"
                        "0  0  5   0  3  0   9  0  0 \n"
                        "0  6  0   7  0  0   0  2  0 ";
    std::stringstream stream(small);
    Sudoku test(9);
    stream >> test;

    SolveStats stats;
    REQUIRE(SmartSolver::Solve(test, stats));
    REQUIRE(!test.HasConflict());
}

/*
TEST_CASE("Solver : Bad Solve", "[]") {
  std::string puzzle =
      "030085000625319700000002005000074100000250000700003002106030009008000010490500860\n";
  std::string expected =
      "934785621625319784817642395562974138341258976789163452156837249278496513493521867\n";
  std::stringstream stream(puzzle);
  Sudoku test(9);
  stream >> test;
  stream.str(expected);
  Sudoku solution(9);
  stream >> solution;
  test.SetSolution(&solution);

  std::stringstream out;

  SolveStats stats;
  auto res = SmartSolver::Solve(test, stats);
  out.str("");
  test.DebugPrint(out);
  INFO(out.str());
  CHECK(res);
  for (unsigned i = 0; i < test.Size(); i++) {
    for (unsigned j = 0; j < test.Size(); j++) {
      out.str("");
      test.DebugPrint(out);
      INFO("Position " << i << " " << j << " " << i*test.Size()+j);
      INFO(out.str());

      CHECK(test[i][j].Value() == expected[i*test.Size()+j]-'0');
    }
  }
}*/

} // namespace sudoku