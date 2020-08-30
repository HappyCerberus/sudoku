/* (c) 2020 RNDr. Simon Toth (happy.cerberus@gmail.com) */

#include "SolveStats.h"
#include <chrono>
#include <ctime>
#include <fstream>
#include <iostream>
#include <sstream>

const char *FISH_NAMES[] = {
    nullptr,        // 0
    nullptr,        // 1
    "X-Wing",       // 2
    "Swordfish",    // 3
    "Jellyfish",    // 4
    "Squirmbag",    // 5
    "Whale",        // 6
    "Leviathan",    // 7
    nullptr
};
SolveStats &SolveStats::operator+=(const SolveStats &stats) {
  for (auto v : stats.groups) {
    groups[v.first] += v.second;
  }
  block_intersections += stats.block_intersections;
  for (auto v : stats.fish) {
    fish[v.first] += v.second;
  }
  for (auto v : stats.finned_fish) {
    finned_fish[v.first] += v.second;
  }
  for (auto v : stats.xchains) {
    xchains[v.first] += v.second;
  }

  xychains += stats.xychains;

  killer_sums += stats.killer_sums;

  return *this;
}
std::ostream &operator<<(std::ostream &s, const SolveStats &stats) {
  s << "Solver stats {" << std::endl;
  s << "\tGroups: ";
  for (unsigned i = 1; i <= 4; i++) {
    unsigned v = 0u;
    if (stats.groups.find(i) != stats.groups.end()) {
      v = stats.groups.at(i);
    }
    s << "(" << i << " : " << v << ")";
  }
  s << std::endl;
  s << "\tIntersections: " << stats.block_intersections << std::endl;
  s << "\tXYChains: " << stats.xychains << std::endl;
  s << "\tKiller block sums: " << stats.killer_sums << std::endl;
  bool header = true;
  for (unsigned i = 2; i <= 7; i++) {
    unsigned v = 0u;
    if (stats.fish.find(i) != stats.fish.end()) {
      if (header) {
        s << "\tFish rules: " << std::endl;
        header = false;
      }
      v = stats.fish.at(i);
      s << "\t\t" << FISH_NAMES[i] << " : " << v << std::endl;
    }
  }
  header = true;
  for (unsigned i = 2; i <= 7; i++) {
    unsigned v = 0u;
    if (stats.finned_fish.find(i) != stats.finned_fish.end()) {
      if (header) {
        s << "\tFinned Fish rules: " << std::endl;
        header = false;
      }
      v = stats.finned_fish.at(i);
      s << "\t\t" << "Finned " << FISH_NAMES[i] << " : " << v << std::endl;
    }
  }
  header = true;
  for (unsigned i = 4; i <= 24; i+=2) {
    unsigned v = 0u;
    if (stats.xchains.find(i) != stats.xchains.end()) {
      if (header) {
        s << "\tXChains rules: " << std::endl;
        header = false;
      }
      v = stats.xchains.at(i);
      s << "\t\t" << "XChain of size " << i << " : " << v << std::endl;
    }
  }
  s << "};" << std::endl;

  return s;
}