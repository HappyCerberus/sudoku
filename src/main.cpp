/* (c) 2020 RNDr. Simon Toth (happy.cerberus@gmail.com) */


#include "Square.h"
#include "Sudoku.h"

#include <chrono>
#include <ctime>
#include <sstream>
#include <vector>
#include <iostream>

// Ultimate goal:
// Sudoku generator, that understands how humans solve Sudoku.
// - can provide difficulty rating
// - can generate from a template
// - support all the variants of Sudoku

bool solve_smart(sudoku::Sudoku &sudoku) {
  while (!sudoku.IsSet()) {
    auto changed_blocks = sudoku.ChangedBlocks();
    if (changed_blocks.size() == 0) {
      return false;
    }
    sudoku.ResetChange();

    for (auto &block : changed_blocks) {
      block->SolveHiddenGroups();
      block->SolveNakedGroups();
    }

    // Intersecting blocks rule
    for (auto &block : changed_blocks) {
      for (auto &rblock : sudoku.Blocks()) {
        block->SolveIntersection(rblock);
      }
    }

    // Swordfish
    for (unsigned i = 2; i <= sudoku.Size() / 2; i++) { // sizes of the
                                                        // swordfish
      for (unsigned j = 1; j <= sudoku.Size(); j++) {
        sudoku.SolveSwordFish(i, j);
      }
    }
  }

  return sudoku.IsSet();
}

// Version 7 is done
// - we can solve easy sudokus
// - implemented inside block set finding
// - implemented block intersections
// - implemented swordfish

// Next steps:
// - unsigned vs size_t cleanup
//   - don't rely on vectors for the size
// - we need tests for block intersections & swordfish
// - design a new puzzle data representation that is suitable for human
//   solving (this would help testing)
// - cleanup interface on BlockChecker & Sudoku

int main() {

  std::string sudoku_9x9 = "4  0  0   0  0  8   0  0  3 \n"
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

  std::string sudoku_16x16 =
      "*  9  *  *   *  *  C  *   *  *  *  *   *  *  *  *"
      "*  D  *  *   *  E  6  *   1  *  *  9   *  4  *  A"
      "*  *  F  5   *  *  *  *   8  6  *  *   1  7  C  9"
      "*  *  C  *   *  *  *  4   3  0  5  7   D  *  B  E"
      "*  *  *  *   *  *  *  6   *  1  *  A   *  *  *  F"
      "*  *  *  *   *  *  9  *   C  *  *  6   *  *  A  *"
      "*  6  *  *   8  *  F  *   *  *  D  *   *  *  *  *"
      "*  4  *  *   D  5  *  1   *  *  B  *   *  C  *  *"
      "*  0  *  *   3  C  *  *   7  *  9  5   B  A  *  *"
      "D  5  *  4   *  *  *  9   0  *  *  F   *  *  *  6"
      "8  *  *  7   *  *  E  *   *  *  *  *   5  *  *  2"
      "*  F  *  2   *  *  *  7   *  *  3  *   C  *  D  *"
      "*  *  B  F   *  *  1  A   5  *  4  *   0  *  *  C"
      "*  1  *  E   *  0  5  *   *  B  7  *   *  *  9  8"
      "C  7  *  6   E  *  8  *   *  *  *  *   *  *  *  3"
      "*  *  5  A   *  *  *  B   *  E  *  8   6  *  2  *";

  {
    std::stringstream stream(sudoku_9x9);
    sudoku::Sudoku test(9);
    stream >> test;

    // record start time
    auto start = std::chrono::system_clock::now();

    if (!solve_smart(test)) {
      std::cout << "There is something very wrong!" << std::endl;
    }

    auto end = std::chrono::system_clock::now();
    std::chrono::duration<double> diff = end - start;
    std::cout << "Time to smart solve 9x9 sudoku " << diff.count()
              << " "
                 "s\n";
    std::cout << test << std::endl;
  }

  std::vector<std::string> puzzles{std::string{"5  0  0   0  0  0   0  6  0 \n"
                                               "0  0  2   0  0  0   0  0  1 \n"
                                               "0  1  0   0  0  0   5  0  0 \n"
                                               "\n"
                                               "0  8  0   0  0  5   3  0  0 \n"
                                               "9  0  0   2  0  0   0  0  0 \n"
                                               "0  0  6   0  4  0   0  0  7 \n"
                                               "\n"
                                               "0  3  0   0  0  9   8  0  0 \n"
                                               "6  0  0   1  0  0   0  3  0 \n"
                                               "0  0  5   0  7  0   0  0  4 "},
                                   std::string{"0  0  0   3  0  0   0  0  9 \n"
                                               "0  0  0   0  2  0   0  3  0 \n"
                                               "0  0  0   0  0  7   2  0  0 \n"
                                               "\n"
                                               "1  0  0   0  0  9   8  0  0 \n"
                                               "0  0  7   4  0  0   0  0  0 \n"
                                               "0  3  0   0  6  0   0  5  0 \n"
                                               "\n"
                                               "0  4  0   0  5  0   0  6  0 \n"
                                               "8  0  0   0  0  3   1  0  0 \n"
                                               "0  0  2   9  0  0   3  0  5 "},
                                   std::string{"7  0  0   0  0  5   0  0  8 \n"
                                               "0  0  1   2  0  0   0  9  0 \n"
                                               "0  6  0   0  3  0   1  0  0 \n"
                                               "\n"
                                               "0  8  0   0  0  0   2  0  0 \n"
                                               "2  0  0   0  0  0   0  0  4 \n"
                                               "0  0  9   0  0  0   0  8  0 \n"
                                               "\n"
                                               "5  0  0   0  0  7   0  0  9 \n"
                                               "0  1  0   0  8  0   6  0  0 \n"
                                               "0  0  0   4  0  0   0  3  0 "},
                                   std::string{"0  0  9   0  0  6   0  1  0 \n"
                                               "0  7  0   2  0  0   3  0  0 \n"
                                               "5  6  0   0  4  0   0  0  7 \n"
                                               "\n"
                                               "2  0  0   0  8  0   0  0  0 \n"
                                               "0  0  5   0  0  1   0  6  0 \n"
                                               "0  3  0   9  0  0   7  0  0 \n"
                                               "\n"
                                               "9  0  0   0  0  0   0  0  5 \n"
                                               "0  4  0   0  0  0   9  0  0 \n"
                                               "0  0  8   0  0  0   0  4  0 "},
                                   std::string{"0  0  3   0  0  5   0  0  9 \n"
                                               "9  0  0   2  0  0   0  6  0 \n"
                                               "0  8  0   0  4  0   7  0  0 \n"
                                               "\n"
                                               "0  0  7   0  0  0   0  0  2 \n"
                                               "0  2  0   0  0  0   1  0  0 \n"
                                               "6  0  0   0  0  0   0  7  0 \n"
                                               "\n"
                                               "0  4  0   0  8  0   6  0  0 \n"
                                               "0  0  0   1  0  0   0  5  0 \n"
                                               "0  0  9   0  0  7   0  0  3 "}};

  for (auto &sudoku : puzzles) {
    std::stringstream stream(sudoku);
    sudoku::Sudoku test(9);
    stream >> test;

    // record start time
    auto start = std::chrono::system_clock::now();

    if (!solve_smart(test)) {
      std::cout << "There is something very wrong!" << std::endl;
    }

    auto end = std::chrono::system_clock::now();
    std::chrono::duration<double> diff = end - start;
    std::cout << "Time to smart solve 9x9 sudoku " << diff.count()
              << " "
                 "s\n";
    std::cout << test << std::endl;
    test.DebugPrint(std::cout);
  }

  {
    std::stringstream stream(sudoku_16x16);
    sudoku::Sudoku test(16);
    stream >> test;

    // record start time
    auto start = std::chrono::system_clock::now();

    if (!solve_smart(test)) {
      std::cout << "There is something very wrong!" << std::endl;
    }

    auto end = std::chrono::system_clock::now();
    std::chrono::duration<double> diff = end - start;
    std::cout << "Time to solve 16x16 pruned sudoku " << diff.count() << " s\n";
    std::cout << test << std::endl;
    test.DebugPrint(std::cout);
  }

  return 0;
}
