//
// Created by Happy on 14/06/2020.
//

#include "SmartSolver.h"

bool SmartSolver::Solve(sudoku::Sudoku &sudoku, SolveStats &stats) {
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