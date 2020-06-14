/* (c) 2020 RNDr. Simon Toth (happy.cerberus@gmail.com) */

#include "Sudoku.h"
#include "Square.h"
#include <iomanip>
#include <ostream>
#include <unordered_map>
#include <iostream>

namespace sudoku {
Sudoku::Sudoku(unsigned size, SudokuTypes type)
    : data_(size, std::vector<sudoku::Square>(size, sudoku::Square{size})),
      block_mapping_(size, std::vector<std::vector<BlockChecker *>>(
                               size, std::vector<BlockChecker *>())),
      size_(size) {
  SetupCheckers(size, type);
}

Sudoku::Sudoku(SudokuDataType data, SudokuTypes type)
    : data_(std::move(data)),
      block_mapping_(data_.size(),
                     std::vector<std::vector<BlockChecker *>>(
                         data_.size(), std::vector<BlockChecker *>())),
      size_(static_cast<unsigned>(data_.size())) {
  SetupCheckers(Size(), type);
}

bool Sudoku::HasConflict() {
  for (const auto &check : checks_) {
    if (check.HasConflict())
      return true;
  }
  return false;
}

void Sudoku::SetupCheckers(unsigned int size, SudokuTypes type) {
  switch (type) {
  case BASIC:
    checks_.reserve(3 * size);
    break;
  case DIAGONAL:
    checks_.reserve(3 * size + 2);
  }

  for (size_t i = 0; i < size; i++) {
    SudokuBlockType row;
    for (size_t j = 0; j < size; j++) {
      row.push_back(&data_[i][j]);
    }
    checks_.emplace_back(row);
    for (size_t j = 0; j < size; j++) {
      block_mapping_[i][j].push_back(&checks_[checks_.size() - 1]);
    }
    row_checks_.push_back(&checks_[checks_.size() - 1]);
  }

  for (size_t j = 0; j < size; j++) {
    SudokuBlockType column;
    for (size_t i = 0; i < size; i++) {
      column.push_back(&data_[i][j]);
    }
    checks_.emplace_back(column);
    for (size_t i = 0; i < size; i++) {
      block_mapping_[i][j].push_back(&checks_[checks_.size() - 1]);
    }
    col_checks_.push_back(&checks_[checks_.size() - 1]);
  }

  std::unordered_map<unsigned, unsigned> blocksizes = {{9, 3}, {16, 4}};
  unsigned bsize = blocksizes[size];
  for (size_t i = 0; i < bsize; i++) {
    for (size_t j = 0; j < bsize; j++) {
      SudokuBlockType block;
      for (size_t x = i * bsize; x < (i + 1) * bsize; x++) {
        for (size_t y = j * bsize; y < (j + 1) * bsize; y++) {
          block.push_back(&data_[x][y]);
        }
      }
      checks_.emplace_back(block);
      for (size_t x = i * bsize; x < (i + 1) * bsize; x++) {
        for (size_t y = j * bsize; y < (j + 1) * bsize; y++) {
          block_mapping_[x][y].push_back(&checks_[checks_.size() - 1]);
        }
      }
    }
  }

  if (type == BASIC)
    return;

  SudokuBlockType d1, d2;
  for (size_t i = 0; i < size; i++) {
    d1.push_back(&data_[i][i]);
    d2.push_back(&data_[i][size - 1 - i]);
  }
  checks_.emplace_back(d1);
  checks_.emplace_back(d2);
  for (size_t i = 0; i < size; i++) {
    block_mapping_[i][i].push_back(&checks_[checks_.size() - 2]);
    block_mapping_[i][size - 1 - i].push_back(&checks_[checks_.size() - 1]);
  }
}

void Sudoku::DebugPrint(std::ostream &s) {
  for (unsigned i = 0; i < Size(); i++) {
    for (unsigned j = 0; j < Size(); j++) {
      s << data_[i][j] << " ";
    }
    s << std::endl;
  }
}
bool Sudoku::HasChange() const {
  for (unsigned i = 0; i < Size(); i++) {
    for (unsigned j = 0; j < Size(); j++) {
      if (data_[i][j].HasChanged())
        return true;
    }
  }
  return false;
}

void Sudoku::ResetChange() {
  for (unsigned i = 0; i < Size(); i++) {
    for (unsigned j = 0; j < Size(); j++) {
      data_[i][j].ResetChanged();
    }
  }
}

std::unordered_set<BlockChecker *> Sudoku::ChangedBlocks() const {
  std::unordered_set<BlockChecker *> result;
  for (unsigned i = 0; i < Size(); i++) {
    for (unsigned j = 0; j < Size(); j++) {
      if (data_[i][j].HasChanged()) {
        for (auto &b : block_mapping_[i][j]) {
          result.insert(b);
        }
      }
    }
  }
  return result;
}

std::pair<unsigned, unsigned> Sudoku::FirstUnset() const {
  for (unsigned i = 0; i < Size(); i++) {
    for (unsigned j = 0; j < Size(); j++) {
      if (!data_[i][j].IsSet())
        return std::make_pair(i, j);
    }
  }
  return std::make_pair(std::numeric_limits<unsigned>::max(),
                        std::numeric_limits<unsigned>::max());
}
bool Sudoku::IsSet() const {
  for (unsigned i = 0; i < Size(); i++) {
    for (unsigned j = 0; j < Size(); j++) {
      if (!data_[i][j].IsSet())
        return false;
    }
  }
  return true;
}

void Sudoku::SolveFish(unsigned int size, unsigned int number) {
  std::vector<std::vector<unsigned>> result;
  recursive_fish_find(result, GetColBlocks(), size, number);
  // process results
  for (const auto &v : result) {
    // the affected rows, remove number from all columns not in set
    std::unordered_set<unsigned> rows;
    for (size_t x : v) {
      GetColBlocks()[x]->NumberPositions(number, rows);
    }
    for (auto r : rows) {
      GetRowBlocks()[r]->Prune(number, v);
    }
  }

  result.clear();
  recursive_fish_find(result, GetRowBlocks(), size, number);
  // process results
  for (const auto &v : result) {
    // the affected columns, remove number from all rows not in set
    std::unordered_set<unsigned> cols;
    // figure out affected columns
    for (size_t x : v) {
      GetRowBlocks()[x]->NumberPositions(number, cols);
    }
    // Remove the number from everywhere except the rows in set
    for (auto r : cols) {
      GetColBlocks()[r]->Prune(number, v);
    }
  }
}

std::pair<unsigned,unsigned> Sudoku::GetFin(const std::vector<BlockChecker *>
    &blocks,
                        const std::vector<unsigned int> &set,
                        unsigned number) const {
  std::unordered_map<unsigned, std::pair<unsigned, std::pair<unsigned,
      unsigned>>> freq;
  for (unsigned i : set) {
    auto pos = blocks[i]->NumberPositions(number);
    for (unsigned j : pos) {
      freq[j].first++;
      freq[j].second.first = i;
      freq[j].second.second = j;
    }
  }

  for (auto i : freq) {
    if (i.second.first == 1)
      return i.second.second;
  }
  return std::make_pair(std::numeric_limits<unsigned>::max(),
                   std::numeric_limits<unsigned>::max());
}

void Sudoku::SolveFinnedFish(unsigned int size, unsigned int number) {
  std::vector<std::vector<unsigned>> result;
  recursive_finned_fish_find(result, GetColBlocks(), size, number);
  // process results
  for (const auto &v : result) {
    // Each of the results is a potential finned fish.
    // We need to find the fin, and check if it is an actual fin or fake one.
    // If it is an actual fin, try to prune the potential positions shared
    // between the fish and the fin.

    // this is a true or a fake fin
    auto fin = GetFin(GetColBlocks(), v, number);

    // this is a true or a fake fin
    if (fin.first == std::numeric_limits<unsigned>::max()) {
      DebugPrint(std::cerr);
    }
    assert(fin.first != std::numeric_limits<unsigned>::max());
    auto pos = GetColBlocks()[fin.first]->NumberPositions(number);
    for (auto j : pos) {
      if (j == fin.second)
        continue;
      if (NumberOfSharedBlocks(std::make_pair(fin.second, fin.first),
                               std::make_pair(j, fin.first)) > 1) {
        bool should_return = false;
        for (unsigned k = 0; k < Size(); k++) {
          if (k == fin.first)
            continue;
          if (NumberOfSharedBlocks(std::make_pair(fin.second, fin.first),
                                   std::make_pair(j, k)) >= 1) {
            data_[j][k] -= number;
            if (data_[j][k].HasChanged()) {
              should_return = true;
            }
          }
        }
        if (should_return) return;
      }
    }
  }

  result.clear();
  recursive_finned_fish_find(result, GetRowBlocks(), size, number);
  // process results
  for (const auto &v : result) {
    // Each of the results is a potential finned fish.
    // We need to find the fin, and check if it is an actual fin or fake one.
    // If it is an actual fin, try to prune the potential positions shared
    // between the fish and the fin.


    // this is a true or a fake fin
    auto fin = GetFin(GetRowBlocks(), v, number);

    // this is a true or a fake fin
    assert(fin.first != std::numeric_limits<unsigned>::max());
    auto pos = GetRowBlocks()[fin.first]->NumberPositions(number);
    for (auto j : pos) {
      if (j == fin.second)
        continue;
      if (NumberOfSharedBlocks(std::make_pair(fin.first, fin.second),
                               std::make_pair(fin.first, j)) > 1) {
        bool should_return = false;
        for (unsigned k = 0; k < Size(); k++) {
          if (k == fin.first)
            continue;
          if (NumberOfSharedBlocks(std::make_pair(fin.first, fin.second),
                                   std::make_pair(k, j)) >= 1) {
            data_[k][j] -= number;
            if (data_[k][j].HasChanged()) {
              should_return = true;
            }
          }
        }
        if (should_return) return;
      }
    }
  }
}

template <typename T>
unsigned CountIntersections(const std::vector<T>& l, const std::vector<T>& r) {
  unsigned count = 0;
  for (auto &li : l) {
    for (auto &lr : r) {
      if (li == lr) count++;
    }
  }
  return count;
}

unsigned Sudoku::NumberOfSharedBlocks(std::pair<unsigned int, unsigned int> l,
                                      std::pair<unsigned int, unsigned int>
                                          r) const {
  return CountIntersections(block_mapping_[l.first][l.second],
block_mapping_[r.first][r.second]);
}

std::ostream &operator<<(std::ostream &s, const Sudoku &puzzle) {
  for (unsigned i = 0; i < puzzle.Size(); i++) {
    for (unsigned j = 0; j < puzzle.Size(); j++) {
      if (puzzle[i][j].Value() >= 10) {
        s << std::setw(2) << static_cast<char>(puzzle[i][j].Value() - 10 + 'A');
      } else {
        s << std::setw(2) << puzzle[i][j].Value();
      }
    }
    s << std::endl;
  }
  return s;
}
// https://stackoverflow.com/questions/799599/c-custom-stream-manipulator-that-changes-next-item-on-stream

/*
*  9  *  *   *  *  C  *   *  *  *  *   *  *  *  *
*  D  *  *   *  E  6  *   1  *  *  9   *  4  *  A
*  *  F  5   *  *  *  *   8  6  *  *   1  7  C  9
*  *  C  *   *  *  *  4   3  0  5  7   D  *  B  E

*  *  *  *   *  *  *  6   *  1  *  A   *  *  *  F
*  *  *  *   *  *  9  *   C  *  *  6   *  *  A  *
*  6  *  *   8  *  F  *   *  *  D  *   *  *  *  *
*  4  *  *   D  5  *  1   *  *  B  *   *  C  *  *

*  0  *  *   3  C  *  *   7  *  9  5   B  A  *  *
D  5  *  4   *  *  *  9   0  *  *  F   *  *  *  6
8  *  *  7   *  *  E  *   *  *  *  *   5  *  *  2
*  F  *  2   *  *  *  7   *  *  3  *   C  *  D  *

*  *  B  F   *  *  1  A   5  *  4  *   0  *  *  C
*  1  *  E   *  0  5  *   *  B  7  *   *  *  9  8
C  7  *  6   E  *  8  *   *  *  *  *   *  *  *  3
*  *  5  A   *  *  *  B   *  E  *  8   6  *  2  *
*/
std::istream &operator>>(std::istream &s, Sudoku &puzzle) {
  std::string debug = "";
  for (unsigned i = 0; i < puzzle.Size(); i++) {
    for (unsigned j = 0; j < puzzle.Size(); j++) {
      char c;
      s >> c;
      debug += c;
      if (c == '*' || c == '0') {
        puzzle[i][j].Reset();
        continue;
      }
      if (isalpha(c))
        puzzle[i][j] = static_cast<unsigned>(c - 'A') + 10u;
      if (isdigit(c))
        puzzle[i][j] = static_cast<unsigned>(c - '0');
    }
  }
  puzzle.debug_ = debug;
  return s;
}
} // namespace sudoku