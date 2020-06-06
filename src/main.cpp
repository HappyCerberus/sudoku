/* (c) 2020 RNDr. Simon Toth (happy.cerberus@gmail.com) */


#include "Square.h"
#include "Sudoku.h"

#include <chrono>
#include <ctime>
#include <iostream>
#include <sstream>
#include <unordered_map>
#include <vector>

// Ultimate goal:
// Sudoku generator, that understands how humans solve Sudoku.
// - can provide difficulty rating
// - can generate from a template
// - support all the variants of Sudoku

struct SolveStats {
  std::unordered_map<unsigned, unsigned> groups;
  unsigned block_intersections;
  std::unordered_map<unsigned, unsigned> swordfish;
};

std::ostream& operator <<(std::ostream& s, const SolveStats& stats) {
  s << "Solver stats {" << std::endl;
  s << "\tGroups: ";
  for (unsigned i = 1; i <= 4; i++) {
    unsigned v = 0u;
    if (stats.groups.find(i) != stats.groups.end()) {
      v = stats.groups.at(i);
    }
    s << "(" << i << " : " << v << ")";
  }
  s << std::endl;
  s << "\tIntersections: " << stats.block_intersections << std::endl;
  s << "\tSwordfish: ";
  for (unsigned i = 2; i <= 8; i++) {
    unsigned v = 0u;
    if (stats.swordfish.find(i) != stats.swordfish.end()) {
      v = stats.swordfish.at(i);
    }
    s << "(" << i << " : " << v << ")";
  }
  s << std::endl;
  s << "};" << std::endl;

  return s;
}


bool solve_smart(sudoku::Sudoku &sudoku, SolveStats& stats) {
  /*
   * while (not_solved) {
   *    while (something_changed) {
   *      solve_for_groups();
   *    }
   *    solve_for_intersections();
   *    if (something_changed) continue;
   *
   *    solve_for_swordfish();
   *    if (something_changed) continue;
   *
   *    make_a_guess();
   * }
   */



  while (!sudoku.IsSet()) {
    auto changed_blocks = sudoku.ChangedBlocks();
    if (changed_blocks.size() == 0) {
      return false;
    }
    sudoku.ResetChange();

    // Rough difficulty scale:
    // Group{1}
    // Group{2}
    // Intersections
    // Group{3}
    // Swordfish{2}
    // Group{4}
    // Swordfish{3,4,5,6,7,8,9}

    for (auto &block : changed_blocks) {
      block->SolveHiddenGroups(1);
      block->SolveNakedGroups(1);
    }

    if (sudoku.HasChange()) {
      stats.groups[1]++;
      continue;
    }

    for (auto &block : sudoku.Blocks()) {
      block.SolveHiddenGroups(2);
      block.SolveNakedGroups(2);
    }

    if (sudoku.HasChange()) {
      stats.groups[2]++;
      continue;
    }

    // Intersecting blocks rule
    for (auto &block : sudoku.Blocks()) {
      for (auto &rblock : sudoku.Blocks()) {
        block.SolveIntersection(rblock);
      }
    }

    if (sudoku.HasChange()) {
      stats.block_intersections++;
      continue;
    }

    for (auto &block : sudoku.Blocks()) {
      block.SolveHiddenGroups(3);
      block.SolveNakedGroups(3);
    }

    if (sudoku.HasChange()) {
      stats.groups[3]++;
      continue;
    }

    for (unsigned j = 1; j <= sudoku.Size(); j++) {
      sudoku.SolveSwordFish(2u, j);
    }

    if (sudoku.HasChange()) {
      stats.swordfish[2]++;
      continue;
    }

    for (auto &block : sudoku.Blocks()) {
      block.SolveHiddenGroups(4);
      block.SolveNakedGroups(4);
    }

    if (sudoku.HasChange()) {
      stats.groups[4]++;
      continue;
    }

    for (unsigned j = 1; j <= sudoku.Size(); j++) {
      sudoku.SolveSwordFish(3u, j);
    }

    if (sudoku.HasChange()) {
      stats.swordfish[3]++;
      continue;
    }

    for (unsigned j = 1; j <= sudoku.Size(); j++) {
      sudoku.SolveSwordFish(4u, j);
    }

    if (sudoku.HasChange()) {
      stats.swordfish[4]++;
      continue;
    }

    for (unsigned j = 1; j <= sudoku.Size(); j++) {
      sudoku.SolveSwordFish(5u, j);
    }

    if (sudoku.HasChange()) {
      stats.swordfish[5]++;
      continue;
    }

    for (unsigned j = 1; j <= sudoku.Size(); j++) {
      sudoku.SolveSwordFish(6u, j);
    }

    if (sudoku.HasChange()) {
      stats.swordfish[6]++;
      continue;
    }

    for (unsigned j = 1; j <= sudoku.Size(); j++) {
      sudoku.SolveSwordFish(7u, j);
    }

    if (sudoku.HasChange()) {
      stats.swordfish[7]++;
      continue;
    }

    for (unsigned j = 1; j <= sudoku.Size(); j++) {
      sudoku.SolveSwordFish(8u, j);
    }

    if (sudoku.HasChange()) {
      stats.swordfish[8]++;
      continue;
    }

    break;
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

    SolveStats stats;
    if (!solve_smart(test, stats)) {
      std::cout << "There is something very wrong!" << std::endl;
    }

    std::cout << stats;

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

    SolveStats stats;
    if (!solve_smart(test, stats)) {
      std::cout << "There is something very wrong!" << std::endl;
    }

    std::cout << stats;

    auto end = std::chrono::system_clock::now();
    std::chrono::duration<double> diff = end - start;
    std::cout << "Time to smart solve 9x9 sudoku " << diff.count()
              << " "
                 "s\n";
    std::cout << test << std::endl;
    test.DebugPrint(std::cout);
  }

  return 0;
}
