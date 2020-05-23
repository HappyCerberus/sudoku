/* (c) 2020 RNDr. Simon Toth (happy.cerberus@gmail.com) */

#include <iostream>

// Ultimate goal:
// Sudoku generator, that understands how humans solve Sudoku.
// - can provide difficulty rating
// - can generate from a template
// - support all the variants of Sudoku

// [DONE] Basic Solver - backtracking

#include "Sudoku.h"
#include <chrono>
#include <ctime>
#include <sstream>
#include <vector>

// Basic Sudoku 9x9
// - rows
// - columns
// - blocks 3x3

// Diagonal
// - basic sudoku
// - 2 diagonals

std::pair<int, int> find_empty(const Sudoku &sudoku) {
  for (size_t i = 0; i < sudoku.data().size(); i++) {
    for (size_t j = 0; j < sudoku.data()[i].size(); j++) {
      if (!sudoku.data()[i][j].IsSet())
        return std::make_pair(i, j);
    }
  }
  return std::make_pair(-1, -1);
}

bool puzzle_filled(const Sudoku &sudoku) {
  for (size_t i = 0; i < sudoku.data().size(); i++) {
    for (size_t j = 0; j < sudoku.data()[i].size(); j++) {
      if (!sudoku.data()[i][j].IsSet())
        return false;
    }
  }

  return true;
}

bool solve_recursive(Sudoku &sudoku) {
  if (!sudoku.CheckPuzzle()) {
    return false;
  }
  if (puzzle_filled(sudoku) && sudoku.CheckPuzzle()) {
    return true;
  }

  auto next = find_empty(sudoku);
  SquareType original = sudoku.data()[next.first][next.second];
  unsigned v = 0;
  while ((v = original.Next(v)) != 0) {
    sudoku.data()[next.first][next.second].Set(v);
    if (solve_recursive(sudoku))
      return true;
  }
  sudoku.data()[next.first][next.second] = original;
  return false;
}

bool solve_smart(Sudoku &sudoku) {
  while (!puzzle_filled(sudoku)) {
    auto changed_blocks = sudoku.ChangedBlocks();
    if (changed_blocks.size() == 0) {
      return false;
    }
    sudoku.ResetChange();

    for (auto &block : changed_blocks) {
      block->Solve();
    }
  }

  return puzzle_filled(sudoku);
}

// Version 3 is done
// We can solve sudokus & diagonal sudokus and not 16x16 sudoku.
// Next steps:
// - stack based non-recursive solution
// - fix the access to the elements of the puzzle Get(int x, int y); operator[]
// - encapsulate the solver a class
// - design a new puzzle data representation that is suitable for human solving

// Solver rules:
// number_of_occurences(Block, Number) == 1
// -> that space needs to be that number
// -> from the perspective of the solver, we can prune all other numbers from
// that space number_of_possibilities(Block) == 1
// -> that space is that number
// -> from the perspective of the solver, we can prune that number from all
// spaces in the block

// if we pick X squares, and the number of possibilities within these squares
// is X -> then these possibilities can't appear anywhere else in the block

// if X possibilities only appear in X squares, then those squares can't
// contain any other numbers

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
    Sudoku test(9);
    stream >> test;

    // record start time
    auto start = std::chrono::system_clock::now();

    if (!solve_recursive(test)) {
      std::cout << "There is something very wrong!" << std::endl;
    }

    auto end = std::chrono::system_clock::now();
    std::chrono::duration<double> diff = end - start;
    std::cout << "Time to solve 9x9 un-pruned sudoku " << diff.count()
              << " s\n";
    std::cout << test << std::endl;
  }

  {
    std::stringstream stream(sudoku_9x9);
    Sudoku test(9);
    stream >> test;

    // record start time
    auto start = std::chrono::system_clock::now();

    for (size_t i = 0; i < test.data().size(); i++) {
      for (size_t j = 0; j < test.data()[i].size(); j++) {
        if (!test.data()[i][j].IsSet())
          continue;
        test.Prune(i, j);
      }
    }

    if (!solve_recursive(test)) {
      std::cout << "There is something very wrong!" << std::endl;
    }

    auto end = std::chrono::system_clock::now();
    std::chrono::duration<double> diff = end - start;
    std::cout << "Time to solve 9x9 pruned sudoku " << diff.count() << " s\n";
    std::cout << test << std::endl;
  }

  {
    std::stringstream stream(sudoku_9x9);
    Sudoku test(9);
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

  {
    std::stringstream stream(sudoku_16x16);
    Sudoku test(16);
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
