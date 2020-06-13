/* (c) 2020 RNDr. Simon Toth (happy.cerberus@gmail.com) */

#include "SolveStats.h"
#include "Square.h"
#include "Sudoku.h"

#include <chrono>
#include <ctime>
#include <fstream>
#include <iostream>
#include <sstream>
#include <unordered_map>
#include <vector>

// Ultimate goal:
// Sudoku generator, that understands how humans solve Sudoku.
// - can provide difficulty rating
// - can generate from a template
// - support all the variants of Sudoku

std::ifstream &seekLines(int64_t offset, std::ifstream &f);
void SolveOneSudoku(std::ifstream &f, int64_t line_number,
                    SolveStats &global_stats, uint64_t &solved,
                    uint64_t &incorrect);

bool solve_smart(sudoku::Sudoku &sudoku, SolveStats &stats) {
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
      sudoku.SolveFish(2u, j);
    }

    if (sudoku.HasChange()) {
      stats.fish[2]++;
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
      sudoku.SolveFish(3u, j);
    }

    if (sudoku.HasChange()) {
      stats.fish[3]++;
      continue;
    }

    for (unsigned j = 1; j <= sudoku.Size(); j++) {
      sudoku.SolveFinnedFish(2u, j);
    }

    if (sudoku.HasChange()) {
      stats.finned_fish[2]++;
      continue;
    }

    for (unsigned j = 1; j <= sudoku.Size(); j++) {
      sudoku.SolveFinnedFish(3u, j);
    }

    if (sudoku.HasChange()) {
      stats.finned_fish[3]++;
      continue;
    }

    for (unsigned j = 1; j <= sudoku.Size(); j++) {
      sudoku.SolveFish(4u, j);
    }

    if (sudoku.HasChange()) {
      stats.fish[4]++;
      continue;
    }

    for (unsigned j = 1; j <= sudoku.Size(); j++) {
      sudoku.SolveFish(5u, j);
    }

    if (sudoku.HasChange()) {
      stats.fish[5]++;
      continue;
    }

    for (unsigned j = 1; j <= sudoku.Size(); j++) {
      sudoku.SolveFish(6u, j);
    }

    if (sudoku.HasChange()) {
      stats.fish[6]++;
      continue;
    }

    for (unsigned j = 1; j <= sudoku.Size(); j++) {
      sudoku.SolveFish(7u, j);
    }

    if (sudoku.HasChange()) {
      stats.fish[7]++;
      continue;
    }

    for (unsigned j = 1; j <= sudoku.Size(); j++) {
      sudoku.SolveFinnedFish(4u, j);
    }

    if (sudoku.HasChange()) {
      stats.finned_fish[4]++;
      continue;
    }

    for (unsigned j = 1; j <= sudoku.Size(); j++) {
      sudoku.SolveFinnedFish(5u, j);
    }

    if (sudoku.HasChange()) {
      stats.finned_fish[5]++;
      continue;
    }

    for (unsigned j = 1; j <= sudoku.Size(); j++) {
      sudoku.SolveFinnedFish(6u, j);
    }

    if (sudoku.HasChange()) {
      stats.finned_fish[6]++;
      continue;
    }

    for (unsigned j = 1; j <= sudoku.Size(); j++) {
      sudoku.SolveFinnedFish(7u, j);
    }

    if (sudoku.HasChange()) {
      stats.finned_fish[7]++;
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

int simple_puzzles() {

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

int run_benchmark(std::ifstream& f, int64_t count) {
  SolveStats global_stats;
  uint64_t solved = 0;
  uint64_t incorrect = 0;

  for (int64_t i = 0; i < count; i++) {
    SolveOneSudoku(f, i, global_stats, solved, incorrect);
  }

  std::cout << "Benchmark results: \t"
               "Solved "
            << solved << " out of " << count
            << " requested.\n"
               "Out of the solved "
            << incorrect
            << " were determined to be "
               "incorrect\n";
  std::cout << global_stats;

  return 0;
}
void SolveOneSudoku(std::ifstream &f, int64_t line_number,
                    SolveStats &global_stats, uint64_t &solved,
                    uint64_t &incorrect) {
  static std::string output;
  output.reserve(81);

  SolveStats stats;
  sudoku::Sudoku s(9, BASIC);
  f >> s;
  char c;
  f >> c;

  if (c != ',')
    std::cerr << "Expected comma after puzzle." << std::endl;
  f >> output;

  if (solve_smart(s, stats)) {
    solved++;
    global_stats += stats;
    unsigned pos = 0;
    bool correct = true;
    for (unsigned x = 0; x < s.Size(); x++) {
      for (unsigned y = 0; y < s.Size(); y++) {
        if (static_cast<unsigned>(output[pos] - '0') != s[x][y].Value())
          correct = false;
        pos++;
      }
    }

    if (!correct) {
      incorrect++;
      std::cerr << "Incorrectly solved puzzle at line " << line_number + 1
                << std::endl;
    }
  }
  output.clear();
}
std::ifstream &seekLines(int64_t offset, std::ifstream &f) {
  std::string line;
  line.reserve(256);
  for (int64_t i = 0; i < offset; i++) {
    if (std::getline(f, line).bad())
      break;
  }

  if (f.bad()) {
    std::cerr << "Unable to seek to the desired line." << std::endl;
  }
  return f;
}

int run_benchmark(const char *filename, int64_t offset, int64_t count) {
  std::ifstream f(filename);
  if (!f.is_open()) {
    std::cerr << "Failed to open file " << filename << std::endl;
  }

  seekLines(offset + 1, f);
  return run_benchmark(f, count);
}

int run_benchmark(const char *filename) {
  std::ifstream f(filename);
  if (!f.is_open()) {
    std::cerr << "Failed to open file " << filename << std::endl;
  }

  SolveStats global_stats;
  uint64_t solved = 0;
  uint64_t incorrect = 0;

  seekLines(1, f);
  int64_t line = 1;
  while (!f.bad() && !f.eof()) {
    SolveOneSudoku(f, line, global_stats, solved, incorrect);
    line++;
    if (line%1000 == 0) {
      std::cout << "At line " << line << std::endl;
    }
  }

  std::cout << "Benchmark results: \t"
               "Solved "
            << solved << " out of " << line-1 <<
            " requested.\n"
               "Out of the solved "
            << incorrect
            << " were determined to be "
               "incorrect\n";
  std::cout << global_stats;

  return 0;
}

int run_benchmark(const char *filename, const char *offset,
                  const char *puzzle_count) {
  char *end = nullptr;
  int64_t off = strtoll(offset, &end, 10);
  if (end == nullptr || *end != '\0') {
    std::cerr << "Unable to interpret offset as number." << std::endl;
    return 1;
  }
  end = nullptr;
  int64_t cnt = strtoll(puzzle_count, &end, 10);
  if (end == nullptr || *end != '\0') {
    std::cerr << "Unable to interpret puzzle count as number." << std::endl;
    return 1;
  }

  return run_benchmark(filename, off, cnt);
}

int main(int argc, char *argv[]) {
  if (argc == 1) {
    return simple_puzzles();
  }

  if (argc == 2) {
    return run_benchmark(argv[1]);
  }

  // filename offset count
  if (argc == 4) {
    return run_benchmark(argv[1], argv[2], argv[3]);
  }

  std::cerr << "Unexpected number of parameters for sudoku.\n"
               "Either call with no parameters, or specify a benchmark file, "
               "offset and number of files to process.\n"
               "./sudoku\n"
               "./sudoku file.csv\n"
               "./sudoku file.csv 0 1000\n"
            << std::endl;
}