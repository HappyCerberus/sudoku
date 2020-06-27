//
// Created by Happy on 14/06/2020.
//

#ifndef SUDOKU_SOLVER_H
#define SUDOKU_SOLVER_H

#include "Sudoku.h"
#include "SolveStats.h"

class SmartSolver {
public:
  static bool Solve(sudoku::Sudoku &sudoku, SolveStats &stats);
  static void SolveGroups(unsigned size, sudoku::Sudoku &sudoku, SolveStats
                                                                     &stats,
                          std::vector<sudoku::Square>& state);
};

#endif // SUDOKU_SOLVER_H
