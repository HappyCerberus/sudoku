/* (c) 2020 RNDr. Simon Toth (happy.cerberus@gmail.com)
 *
 * Human-like Sudoku solver.
 *
 * This solver employs human solving techniques to solve Sudoku puzzles and
 * does not employ any backtracking or any other form of guessing.
 */

#include "SolveStats.h"
#include "Square.h"
#include "Sudoku.h"

#include "Progressbar.h"
#include "SmartSolver.h"
#include <chrono>
#include <ctime>
#include <fstream>
#include <iostream>
#include <sstream>
#include <unordered_map>
#include <vector>
#include <chrono>
#include <thread>

// Ultimate goal:
// Sudoku generator, that understands how humans solve Sudoku.
// - can provide difficulty rating
// - can generate from a template
// - support all the variants of Sudoku

std::ifstream &seekLines(int64_t offset, std::ifstream &f);
void SolveOneSudoku(std::ifstream &f, int64_t line_number,
                    SolveStats &global_stats, uint64_t &solved,
                    uint64_t &incorrect);

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

int run_benchmark(std::ifstream& f, int64_t count) {
  SolveStats global_stats;
  uint64_t solved = 0;
  uint64_t incorrect = 0;

  {
    Progressbar x(static_cast<int>(count), std::cout, 79u);
    for (int64_t i = 0; i < count; i++) {
      SolveOneSudoku(f, i, global_stats, solved, incorrect);
      x.Step(1);
    }
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

  if (SmartSolver::Solve(s, stats)) {
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
      std::cerr << output << std::endl;
    }
  }/* else {
    std::cerr << std::endl;
    std::cerr << "Unsolved puzzle at line " << line_number << std::endl;
    std::cerr << output << std::endl;
  }*/
  output.clear();
}
/*
void SolveOneSudokuWithoutSolution(std::ifstream &f, int64_t line_number,
                    SolveStats &global_stats, uint64_t &solved,
                    uint64_t &incorrect) {
  SolveStats stats;
  sudoku::Sudoku s(9, BASIC);
  f >> s;

  if (SmartSolver::Solve(s, stats)) {
    solved++;
    global_stats += stats;
  }
}
*/
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
/*
int run_benchmark_no_solutions(const char *filename) {
  std::ifstream f(filename);
  if (!f.is_open()) {
    std::cerr << "Failed to open file " << filename << std::endl;
  }

  SolveStats global_stats;
  uint64_t solved = 0;
  uint64_t incorrect = 0;

  int64_t line = 0;
  while (!f.bad() && !f.eof()) {
    SolveOneSudokuWithoutSolution(f, line, global_stats, solved, incorrect);
    line++;
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
*/
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
  if (argc == 2) {
    return run_benchmark(argv[1]);
  }

//  if (argc == 3) {
//    return run_benchmark_no_solutions(argv[1]);
//  }

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
