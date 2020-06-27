//
// Created by Happy on 14/06/2020.
//

#include "SmartSolver.h"

std::vector<sudoku::Square> EmptyState(unsigned size) {
  return std::vector<sudoku::Square>(size*size, sudoku::Square(size));
}

bool SmartSolver::Solve(sudoku::Sudoku &sudoku, SolveStats &stats) {
  std::vector<sudoku::Square> groups[4];
  groups[0] = std::move(EmptyState(sudoku.Size()));
  groups[1] = std::move(EmptyState(sudoku.Size()));
  groups[2] = std::move(EmptyState(sudoku.Size()));
  groups[3] = std::move(EmptyState(sudoku.Size()));

  while (!sudoku.IsSet()) {
    auto changed_blocks = sudoku.ChangedBlocks();
    if (changed_blocks.size() == 0) {
      return false;
      }
    sudoku.ResetChange();


    SolveGroups(1, sudoku, stats, groups[0]);
    SolveGroups(2, sudoku, stats, groups[1]);

    // Intersecting blocks rule
    for (auto &block : sudoku.Blocks()) {
      for (auto &rblock : sudoku.Blocks()) {
        if (&block != &rblock)
          block.SolveIntersection(rblock);
      }
    }

    if (sudoku.HasChange()) {
      stats.block_intersections++;
      continue;
    }

    SolveGroups(3, sudoku, stats, groups[2]);

    for (unsigned j = 1; j <= sudoku.Size(); j++) {
      sudoku.SolveFish(2u, j);
    }

    if (sudoku.HasChange()) {
      stats.fish[2]++;
      continue;
    }

    SolveGroups(4, sudoku, stats, groups[3]);

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

    break;
  }

  return sudoku.IsSet();
}
void SmartSolver::SolveGroups(unsigned int size, sudoku::Sudoku &sudoku,
                              SolveStats &stats,
                              std::vector<sudoku::Square> &state) {
  auto changed_blocks = sudoku.ChangedBlocks(state);
  auto stored = sudoku.State();

  for (auto &block : changed_blocks) {
    auto removed_numbers = sudoku.RemovedNumbers(block, state);
    if (removed_numbers.size() != 0)
      block->SolveHiddenGroups(size, removed_numbers);
    auto interesting_squares = sudoku.ChangedSquares(block, state);
    if (interesting_squares.size() != 0)
      block->SolveNakedGroups(size, interesting_squares);
  }

  if (sudoku.HasChange(stored)) {
    stats.groups[size]++;
  }

  state = std::move(stored);
}


