/* (c) 2020 RNDr. Simon Toth (happy.cerberus@gmail.com) */

#ifndef SUDOKU_SOLVER_H
#define SUDOKU_SOLVER_H

#include "Sudoku.h"
#include "SolveStats.h"

class SmartSolver {
public:
  static bool SingleStep(sudoku::Sudoku &sudoku, SolveStats &stats);
  static bool Solve(sudoku::Sudoku &sudoku, SolveStats &stats);
};

#endif // SUDOKU_SOLVER_H
