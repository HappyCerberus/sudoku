/* (c) 2020 RNDr. Simon Toth (happy.cerberus@gmail.com) */

#ifndef SUDOKU_SOLVESTATS_H
#define SUDOKU_SOLVESTATS_H

#include <unordered_map>
#include <vector>
#include <iosfwd>

struct SolveStats {
  std::unordered_map<unsigned, unsigned> groups;
  unsigned block_intersections;
  std::unordered_map<unsigned, unsigned> fish;
  std::unordered_map<unsigned, unsigned> finned_fish;
  std::unordered_map<unsigned, unsigned> xchains;
  unsigned xychains;
  unsigned killer_sums;
  SolveStats() : groups(), block_intersections(0), fish(), finned_fish(),
                 xchains(), xychains(0), killer_sums(0) {}
  SolveStats &operator+=(const SolveStats &stats);
};

std::ostream &operator<<(std::ostream &s, const SolveStats &stats);

#endif // SUDOKU_SOLVESTATS_H
