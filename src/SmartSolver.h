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
};

#endif // SUDOKU_SOLVER_H
