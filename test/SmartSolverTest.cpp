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
  CHECK(SmartSolver::Solve(test3, stats));
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
    std::string solution = "327 189 654 684 573 219 951 426 873 136 847 592 748 295 136 592 361 487 413 952 768 275 638 941 869 714 325";
    stream.str(solution);
    Sudoku solved(9u);
    stream >> solved;

    test.SetSolution(&solved);

    SolveStats stats;
    while (!test.IsSet() && SmartSolver::SingleStep(test,stats));
    WARN(test.DebugString());
    REQUIRE(!test.HasConflict());
    REQUIRE(test.IsSet());
}


TEST_CASE("Solver : Bad Solve", "[]") {
  std::string puzzle =
      "030 085 000 625 319 700 000 002 005 000 074 100 000 250 000 700 003 002 106 030 009 008 000 010 490 500 860\n";
  std::string expected = "934785621625319784817642395562974138341258976789163452156837249278496513493521867\n";
  std::stringstream stream(puzzle);
  Sudoku test(9);
  stream >> test;
  stream.str(expected);
  Sudoku solution(9);
  stream >> solution;
  test.SetSolution(&solution);

  std::stringstream out;

  SolveStats stats;
  while (!test.IsSet() && !test.HasConflict() && SmartSolver::SingleStep(test,stats));
  REQUIRE(!test.HasConflict());
  /*
  for (unsigned i = 0; i < test.Size(); i++) {
    for (unsigned j = 0; j < test.Size(); j++) {
      CHECK(test[i][j].SingletonValue() == expected[i*test.Size()+j]-'0');
    }
  }*/
}

} // namespace sudoku