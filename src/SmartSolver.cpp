/* (c) 2020 RNDr. Simon Toth (happy.cerberus@gmail.com) */

#include "SmartSolver.h"
#include "core/SudokuAlgorithms.h"
#include <iostream>


bool SmartSolver::SingleStep(sudoku::Sudoku &sudoku, SolveStats &stats) {
    if (sudoku.HasSolution()) {
      if (!sudoku.CheckAgainstSolution())
        return false;
    }

    auto changed_blocks = sudoku.ChangedBlocks();
    if (changed_blocks.size() == 0) {
      return false;
    }
    sudoku.ResetChange();

    for (auto &block : changed_blocks) {
      SolveHiddenGroups(block->GetSquares(), 1);
      SolveNakedGroups(block->GetSquares(), 1);
    }

    if (sudoku.HasChange()) {
      stats.groups[1]++;
      return true;
    }

    for (auto &block : sudoku.Blocks()) {
      SolveHiddenGroups(block.GetSquares(), 2);
      SolveNakedGroups(block.GetSquares(), 2);
    }

    if (sudoku.HasSolution()) {
      if (!sudoku.CheckAgainstSolution())
        return false;
    }

    if (sudoku.HasChange()) {
      stats.groups[2]++;
      return true;
    }

    // Limit squares to only possible sums of killer blocks.
    sudoku.PruneKillerBlockSums();
    sudoku.PruneSquaresFromKillerBlocks();

    if (sudoku.HasSolution()) {
        if (!sudoku.CheckAgainstSolution())
            return false;
    }

    if (sudoku.HasChange()) {
        stats.killer_sums++;
        return true;
    }

    // Intersecting blocks rule
    for (auto &block : sudoku.Blocks()) {
      for (auto &rblock : sudoku.Blocks()) {
        SolveBlockIntersection(block, rblock);
      }
    }

    if (sudoku.HasSolution()) {
      if (!sudoku.CheckAgainstSolution())
        return false;
    }

    if (sudoku.HasChange()) {
      stats.block_intersections++;
      return true;
    }

    for (auto &block : sudoku.Blocks()) {
      SolveHiddenGroups(block.GetSquares(), 3);
      SolveNakedGroups(block.GetSquares(), 3);
    }

    if (sudoku.HasSolution()) {
      if (!sudoku.CheckAgainstSolution())
        return false;
    }

    if (sudoku.HasChange()) {
      stats.groups[3]++;
      return true;
    }

    for (unsigned j = 1; j <= sudoku.Size(); j++) {
      sudoku.SolveFish(2u, j);
    }

    if (sudoku.HasSolution()) {
      if (!sudoku.CheckAgainstSolution())
        return false;
    }

    if (sudoku.HasChange()) {
      stats.fish[2]++;
      return true;
    }

    for (unsigned j = 1; j <= sudoku.Size(); j++) {
      sudoku.SolveXChains(4u, j);
    }

    if (sudoku.HasSolution()) {
      if (!sudoku.CheckAgainstSolution())
        return false;
    }

    if (sudoku.HasChange()) {
      stats.xchains[4u]++;
      return true;
    }

    for (auto &block : sudoku.Blocks()) {
      SolveHiddenGroups(block.GetSquares(), 4);
      SolveNakedGroups(block.GetSquares(), 4);
    }

    if (sudoku.HasSolution()) {
      if (!sudoku.CheckAgainstSolution())
        return false;
    }

    if (sudoku.HasChange()) {
      stats.groups[4]++;
      return true;
    }

    for (unsigned j = 1; j <= sudoku.Size(); j++) {
      sudoku.SolveFish(3u, j);
    }

    if (sudoku.HasSolution()) {
      if (!sudoku.CheckAgainstSolution())
        return false;
    }

    if (sudoku.HasChange()) {
      stats.fish[3]++;
      return true;
    }

    for (unsigned j = 1; j <= sudoku.Size(); j++) {
      sudoku.SolveFinnedFish(2u, j);
    }

    if (sudoku.HasSolution()) {
      if (!sudoku.CheckAgainstSolution())
        return false;
    }

    if (sudoku.HasChange()) {
      stats.finned_fish[2]++;
      return true;
    }

    for (unsigned j = 1; j <= sudoku.Size(); j++) {
      sudoku.SolveXChains(6u, j);
    }

    if (sudoku.HasSolution()) {
      if (!sudoku.CheckAgainstSolution())
        return false;
    }

    if (sudoku.HasChange()) {
      stats.xchains[6]++;
      return true;
    }

    for (unsigned j = 1; j <= sudoku.Size(); j++) {
      sudoku.SolveFinnedFish(3u, j);
    }

    if (sudoku.HasSolution()) {
      if (!sudoku.CheckAgainstSolution())
        return false;
    }

    if (sudoku.HasChange()) {
      stats.finned_fish[3]++;
      return true;
    }

    for (unsigned j = 1; j <= sudoku.Size(); j++) {
      sudoku.SolveFish(4u, j);
    }

    if (sudoku.HasSolution()) {
      if (!sudoku.CheckAgainstSolution())
        return false;
    }

    if (sudoku.HasChange()) {
      stats.fish[4]++;
      return true;
    }

    for (unsigned j = 1; j <= sudoku.Size(); j++) {
      sudoku.SolveFish(5u, j);
    }

    if (sudoku.HasSolution()) {
      if (!sudoku.CheckAgainstSolution())
        return false;
    }

    if (sudoku.HasChange()) {
      stats.fish[5]++;
      return true;
    }

    for (unsigned j = 1; j <= sudoku.Size(); j++) {
      sudoku.SolveFish(6u, j);
    }

    if (sudoku.HasSolution()) {
      if (!sudoku.CheckAgainstSolution())
        return false;
    }
   
    if (sudoku.HasChange()) {
      stats.fish[6]++;
      return true;
    }

    for (unsigned j = 1; j <= sudoku.Size(); j++) {
      sudoku.SolveFish(7u, j);
    }

    if (sudoku.HasSolution()) {
      if (!sudoku.CheckAgainstSolution())
        return false;
    }

    if (sudoku.HasChange()) {
      stats.fish[7]++;
      return true;
    }

    for (unsigned j = 1; j <= sudoku.Size(); j++) {
      sudoku.SolveXChains(8u, j);
    }

    if (sudoku.HasSolution()) {
      if (!sudoku.CheckAgainstSolution())
        return false;
    }

    if (sudoku.HasChange()) {
      stats.xchains[8]++;
      return true;
    }

    for (unsigned j = 1; j <= sudoku.Size(); j++) {
      sudoku.SolveFinnedFish(4u, j);
    }

    if (sudoku.HasSolution()) {
      if (!sudoku.CheckAgainstSolution())
        return false;
    }

    if (sudoku.HasChange()) {
      stats.finned_fish[4]++;
      return true;
    }

    for (unsigned j = 1; j <= sudoku.Size(); j++) {
      sudoku.SolveXChains(10u, j);
    }

    if (sudoku.HasSolution()) {
      if (!sudoku.CheckAgainstSolution())
        return false;
    }

    if (sudoku.HasChange()) {
      stats.xchains[10]++;
      return true;
    }

    sudoku.SolveXYChains();

    if (sudoku.HasSolution()) {
      if (!sudoku.CheckAgainstSolution())
        return false;
    }

    if (sudoku.HasChange()) {
      stats.xychains++;
      return true;
    }

    for (unsigned j = 1; j <= sudoku.Size(); j++) {
      sudoku.SolveFinnedFish(5u, j);
    }

    if (sudoku.HasSolution()) {
      if (!sudoku.CheckAgainstSolution())
        return false;
    }

    if (sudoku.HasChange()) {
      stats.finned_fish[5]++;
      return true;
    }

    for (unsigned j = 1; j <= sudoku.Size(); j++) {
      sudoku.SolveFinnedFish(6u, j);
    }

    if (sudoku.HasSolution()) {
      if (!sudoku.CheckAgainstSolution())
        return false;
    }

    if (sudoku.HasChange()) {
      stats.finned_fish[6]++;
      return true;
    }

    for (unsigned j = 1; j <= sudoku.Size(); j++) {
      sudoku.SolveFinnedFish(7u, j);
    }

    if (sudoku.HasSolution()) {
      if (!sudoku.CheckAgainstSolution())
        return false;
    }

    if (sudoku.HasChange()) {
      stats.finned_fish[7]++;
      return true;
    }

    return false;
}

bool SmartSolver::Solve(sudoku::Sudoku &sudoku, SolveStats &stats) {
  while (!sudoku.IsSet() && SingleStep(sudoku, stats));
  return sudoku.IsSet();
}