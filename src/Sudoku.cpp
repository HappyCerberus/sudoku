/* (c) 2020 RNDr. Simon Toth (happy.cerberus@gmail.com) */

#include "Sudoku.h"
#include "Square.h"
#include <functional>
#include <iomanip>
#include <iostream>
#include <ostream>
#include <iterator>
#include <unordered_map>

namespace sudoku {
Sudoku::Sudoku(unsigned size, SudokuTypes type)
    : data_(size*size, sudoku::Square{size}),
      block_mapping_(size, std::vector<std::vector<BlockChecker *>>(
                               size, std::vector<BlockChecker *>())),
      size_(size), puzzle_type_(type) {
  SetupCheckers(size, type);
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
    std::vector<::sudoku::Square *> row;
    for (size_t j = 0; j < size; j++) {
      row.push_back(&data_[i*Size()+j]);
    }
    checks_.emplace_back(row);
    for (size_t j = 0; j < size; j++) {
      block_mapping_[i][j].push_back(&checks_[checks_.size() - 1]);
    }
    row_checks_.push_back(&checks_[checks_.size() - 1]);
  }

  for (size_t j = 0; j < size; j++) {
    std::vector<::sudoku::Square *> column;
    for (size_t i = 0; i < size; i++) {
      column.push_back(&data_[i*Size()+j]);
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
      std::vector<::sudoku::Square *> block;
      for (size_t x = i * bsize; x < (i + 1) * bsize; x++) {
        for (size_t y = j * bsize; y < (j + 1) * bsize; y++) {
          block.push_back(&data_[x*Size()+y]);
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

  std::vector<::sudoku::Square *> d1, d2;
  for (size_t i = 0; i < size; i++) {
    d1.push_back(&data_[i*Size()+i]);
    d2.push_back(&data_[i*Size()+size - 1 - i]);
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
      s << data_[i*Size()+j] << " ";
    }
    s << std::endl;
  }
}
bool Sudoku::HasChange() const {
  for (unsigned i = 0; i < Size(); i++) {
    for (unsigned j = 0; j < Size(); j++) {
      if (data_[i*Size()+j].HasChanged())
        return true;
    }
  }
  return false;
}

void Sudoku::ResetChange() {
  for (unsigned i = 0; i < Size(); i++) {
    for (unsigned j = 0; j < Size(); j++) {
      data_[i*Size()+j].ResetChanged();
    }
  }
}

std::unordered_set<BlockChecker *> Sudoku::ChangedBlocks() const {
  std::unordered_set<BlockChecker *> result;
  for (unsigned i = 0; i < Size(); i++) {
    for (unsigned j = 0; j < Size(); j++) {
      if (data_[i*Size()+j].HasChanged()) {
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
      if (!data_[i*Size()+j].IsSet())
        return std::make_pair(i, j);
    }
  }
  return std::make_pair(std::numeric_limits<unsigned>::max(),
                        std::numeric_limits<unsigned>::max());
}
bool Sudoku::IsSet() const {
  for (unsigned i = 0; i < Size(); i++) {
    for (unsigned j = 0; j < Size(); j++) {
      if (!data_[i*Size()+j].IsSet())
        return false;
    }
  }
  return true;
}

void Sudoku::SolveFish(unsigned int size, unsigned int number) {
  std::vector<unsigned> result;
  recursive_fish_find(result, GetColBlocks(), size, number);
  // process results
  for (size_t i = 0; i < result.size() / size; i++) {
    // the affected rows, remove number from all columns not in set
    std::unordered_set<unsigned> rows;
    for (size_t j = 0; j < size; j++) {
      GetColBlocks()[result[i * size + j]]->NumberPositions(number, rows);
    }
    for (auto r : rows) {
      auto b = result.begin();
      std::advance(b, static_cast<std::vector<unsigned>::difference_type>(i*size));
      auto e = result.begin();
      std::advance(e, static_cast<std::vector<unsigned>::difference_type>((i+1)*size));
      GetRowBlocks()[r]->Prune(number, b, e);
    }
  }

  result.clear();
  recursive_fish_find(result, GetRowBlocks(), size, number);
  // process results
  for (size_t i = 0; i < result.size() / size; i++) {
    // the affected rows, remove number from all columns not in set
    std::unordered_set<unsigned> cols;
    for (size_t j = 0; j < size; j++) {
      GetRowBlocks()[result[i * size + j]]->NumberPositions(number, cols);
    }
    // Remove the number from everywhere except the rows in set
    for (auto r : cols) {
      auto b = result.begin();
      advance(b, static_cast<std::vector<unsigned>::difference_type>(i*size));
      auto e = result.begin();
      advance(e, static_cast<std::vector<unsigned>::difference_type>((i+1)*size));
      GetColBlocks()[r]->Prune(number, b, e);
    }
  }
}

std::pair<unsigned, unsigned>
Sudoku::GetFin(const std::vector<BlockChecker *> &blocks,
               std::vector<unsigned>::const_iterator begin,
               std::vector<unsigned>::const_iterator end, unsigned number) const {
  std::unordered_map<unsigned,
                     std::pair<unsigned, std::pair<unsigned, unsigned>>>
      freq;
  for (auto i = begin; i != end; i++) {
    auto pos = blocks[*i]->NumberPositions(number);
    for (unsigned j : pos) {
      freq[j].first++;
      freq[j].second.first = *i;
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
  std::vector<unsigned> result;
  recursive_finned_fish_find(result, GetColBlocks(), size, number);
  // process results
  for (size_t i = 0; i < result.size()/size; i++){
    // Each of the results is a potential finned fish.
    // We need to find the fin, and check if it is an actual fin or fake one.
    // If it is an actual fin, try to prune the potential positions shared
    // between the fish and the fin.

    // this is a true or a fake fin
    auto b = result.begin();
    advance(b, static_cast<std::vector<unsigned>::difference_type>(i*size));
    auto e = result.begin();
    advance(e, static_cast<std::vector<unsigned>::difference_type>((i+1)*size));
    auto fin = GetFin(GetColBlocks(), b, e, number);

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
          bool skip = false;
          for (auto x = b; x != e; x++) {
            if (k == *x)
              skip = true;
          }
          if (skip || k == fin.first)
            continue;
          if (NumberOfSharedBlocks(std::make_pair(fin.second, fin.first),
                                   std::make_pair(j, k)) >= 1) {
            data_[j*Size()+k] -= number;
            if (data_[j*Size()+k].HasChanged()) {
              should_return = true;
            }
          }
        }
        if (should_return)
          return;
      }
    }
  }

  result.clear();
  recursive_finned_fish_find(result, GetRowBlocks(), size, number);
  // process results
  for (size_t i = 0; i < result.size()/size; i++) {
    // Each of the results is a potential finned fish.
    // We need to find the fin, and check if it is an actual fin or fake one.
    // If it is an actual fin, try to prune the potential positions shared
    // between the fish and the fin.

    // this is a true or a fake fin
    auto b = result.begin();
    advance(b, static_cast<std::vector<unsigned>::difference_type>(i*size));
    auto e = result.begin();
    advance(e, static_cast<std::vector<unsigned>::difference_type>((i+1)*size)) ;
    auto fin = GetFin(GetRowBlocks(), b, e, number);

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
          bool skip = false;
          for (auto x = b; x != e; x++) {
            if (k == *x)
              skip = true;
          }
          if (skip || k == fin.first)
            continue;
          if (NumberOfSharedBlocks(std::make_pair(fin.first, fin.second),
                                   std::make_pair(k, j)) >= 1) {
            data_[k*Size()+j] -= number;
            if (data_[k*Size()+j].HasChanged()) {
              should_return = true;
            }
          }
        }
        if (should_return)
          return;
      }
    }
  }
}

template <typename T>
unsigned CountIntersections(const std::vector<T> &l, const std::vector<T> &r) {
  unsigned count = 0;
  for (auto &li : l) {
    for (auto &lr : r) {
      if (li == lr)
        count++;
    }
  }
  return count;
}

unsigned
Sudoku::NumberOfSharedBlocks(std::pair<unsigned int, unsigned int> l,
                             std::pair<unsigned int, unsigned int> r) const {
  return CountIntersections(block_mapping_[l.first][l.second],
                            block_mapping_[r.first][r.second]);
}

ChainsGraph Sudoku::GetChains(unsigned number) const {
  ChainsGraph result;
  for (unsigned i = 0; i < Size(); i++) {
    for (unsigned j = 0; j < Size(); j++) {
      if (data_[i*Size()+j].IsPossible(number)) {
        result.nodes.push_back(i*Size()+j);
      }
    }
  }
  for (const auto& b : checks_) {
    auto pos = b.NumberPositions(number);
    if (pos.size() == 2) {
      auto i = pos.begin();
      Square* first = b.GetSquare(*i);
      i++;
      Square* second = b.GetSquare(*i);

      result.strong_links.insert({first-&data_[0], second-&data_[0]});
      result.weak_links.insert({first-&data_[0], second-&data_[0]});
      result.strong_links.insert({second-&data_[0], first-&data_[0]});
      result.weak_links.insert({second-&data_[0], first-&data_[0]});
    } else if (pos.size() > 2) {
      for (auto i : pos) {
        for (auto j : pos) {
          if (i == j) continue;

          Square* first = b.GetSquare(i);
          Square* second = b.GetSquare(j);
          result.weak_links.insert({first-&data_[0], second-&data_[0]});
        }
      }
    }
  }
  return result;
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

void Sudoku::dfs_traverse(const ChainsGraph& g, unsigned length,
                  const std::function<void(const std::vector<unsigned>&)> &cb,
                  std::vector<unsigned> &path, bool weak) {
  if (path.size() == length) {
    cb(path);
    return;
  }

  unsigned node = path[path.size()-1];
  std::pair<std::unordered_multimap<unsigned,unsigned>::const_iterator,
            std::unordered_multimap<unsigned,unsigned>::const_iterator> range;

  if (weak) {
    range = g.weak_links.equal_range(node);
  } else {
    range = g.strong_links.equal_range(node);
  }

  for (auto i = range.first; i != range.second; i++) {
    bool found = false;
    for (auto j : path) {
      if (j == i->second) found = true;
    }
    if (found) continue;
    path.push_back(i->second);
    dfs_traverse(g, length, cb, path, !weak);
    path.pop_back();
  }
}

void Sudoku::dfs_traverse(const ChainsGraph& g, unsigned length,
                  const std::function<void(const std::vector<unsigned>&)> &cb) {
  std::vector<unsigned> path;
  for (auto n : g.nodes) {
    path.push_back(n);
    dfs_traverse(g, length, cb, path, false);
    path.pop_back();
  }
}

void Sudoku::SolveXChains(unsigned length, unsigned number) {
  auto graph = GetChains(number);
  auto cb = [this, number](const std::vector<unsigned> &path){
    this->PruneNumbersSeenFrom(path, number);
  };
  dfs_traverse(graph, length, cb);
}

bool Sudoku::dfs_traverse(const std::vector<ChainsGraph>& g, const
                      std::function<bool
    (const std::vector<unsigned>&)> &cb, std::vector<unsigned> &path,
                  unsigned number, unsigned next_number) {
  if (next_number == number) {
    return cb(path);
  }

  unsigned node = path[path.size()-1];
  std::pair<std::unordered_multimap<unsigned,unsigned>::const_iterator,
      std::unordered_multimap<unsigned,unsigned>::const_iterator> range;

  range = g[next_number-1].weak_links.equal_range(node);
  for (auto i = range.first; i != range.second; i++) {
    bool found = false;
    for (auto j : path) {
      if (j == i->second) found = true;
    }
    if (found) continue;

    if (data_[i->second].CountPossible() != 2) continue;

    // what is the other number in the square != source_number
    unsigned next = 0;
    while ((next = data_[i->second].Next(next)) == next_number);
    if (next == 0) {
      abort(); // something completely broken
    }

    path.push_back(i->second);
    if (dfs_traverse(g, cb, path, number, next))
      return true;
    path.pop_back();
  }
  return false;
}

bool Sudoku::dfs_traverse(const std::vector<ChainsGraph>& g, const
                          std::function<bool(const
                                                                 std::vector<unsigned>&)> &cb,
                  unsigned number) {
  std::vector<unsigned> path;
  for (auto n : g[number-1].nodes) {
    if (data_[n].CountPossible() == 2) {
      path.push_back(n);
      // what is the other number in the square != source_number
      unsigned next = 0;
      while ((next = data_[n].Next(next)) == number);
      if (next == 0) {
        abort(); // something completely broken
      }
      if (dfs_traverse(g, cb, path, number, next))
        return true;
      path.pop_back();
    }
  }
  return false;
}

void Sudoku::SolveXYChains() {
  std::vector<ChainsGraph> graphs;
  for (unsigned i = 1; i <= Size(); i++) {
    graphs.push_back(GetChains(i));
  }

  for (unsigned number = 1; number <= Size(); number++) {
    auto cb = [this, number](const std::vector<unsigned> &path) -> bool {
      return this->PruneNumbersSeenFrom(path, number);
    };
    if (dfs_traverse(graphs, cb, number)) return;
  }
}

bool Sudoku::PruneNumbersSeenFrom(const std::vector<unsigned>& path, unsigned
                                                                         number) {
  unsigned begin = path[0];
  unsigned end = path[path.size()-1];

  const auto &bb = block_mapping_[begin/Size()][begin%Size()];
  const auto &be = block_mapping_[end/Size()][end%Size()];

  bool modified = false;

  std::unordered_set<Square *> bs;
  for (auto s : bb) {
    for (unsigned i = 0; i < Size(); i++) {
      bool skip = false;
      for (auto j : path) {
        if (&data_[j] == s->GetSquare(i)) skip = true;
      }
      if (skip) continue;
      bs.insert(s->GetSquare(i));
    }
  }

  for (auto s : be) {
    for (unsigned i = 0; i < Size(); i++) {
      bool skip = false;
      for (auto j : path) {
        if (&data_[j] == s->GetSquare(i)) skip = true;
      }
      if (skip) continue;
      if (bs.find(s->GetSquare(i)) != bs.end()) {
        (*s->GetSquare(i)) -= number;
        modified = true;
      }
    }
  }

  return modified;
}

std::string Sudoku::Serialize() const {
    std::stringstream s;
    // version:
    s << "0:";
    // num_rows:num_cols:num_possibilities:
    s << Size() << ":" << Size() << ":" << Size() << ":";
    for (unsigned i = 0; i < Size(); i++) {
        for (unsigned j = 0; j < Size(); j++) {
            data_[i*Size()+j].Serialize(s);
            s << ":";
        }
    }
    // puzzle type
    s << static_cast<int>(puzzle_type_) << ":";
    return s.str();
}

void Sudoku::Deserialize(const std::string &data) {
    std::stringstream s;
    s.str(data);
    char delim = 0;
    int version = 0;
    s >> version >> delim;
    if (version != 0)
        throw std::out_of_range("Currently only version 0 of data format is supported.");
    if (!s)
        throw std::runtime_error("Unexpected data in deserialized puzzle.");
    unsigned rows = 0, cols = 0, poss = 0;
    s >> rows >> delim >> cols >> delim >> poss >> delim;
    if (!(rows == cols && cols == poss))
        throw std::out_of_range("Currently on perfectly square sudoku puzzles are supported.");
    if (!s)
        throw std::runtime_error("Unexpected data in deserialized puzzle.");
    size_ = rows;
    data_ = std::vector<sudoku::Square>(size_*size_, sudoku::Square{size_});
    block_mapping_ = std::vector<std::vector<std::vector<BlockChecker *>>>(size_, std::vector<std::vector<BlockChecker *>>(size_, std::vector<BlockChecker *>()));
    checks_.clear();
    row_checks_.clear();
    col_checks_.clear();

    for (unsigned i = 0; i < Size(); i++) {
        for (unsigned j = 0; j < Size(); j++) {
            data_[i*Size()+j].Deserialize(s);
            s >> delim;
            if (!s)
                throw std::runtime_error("Unexpected data in deserialized puzzle.");
        }
    }

    unsigned type = 0;
    s >> type;
    if (type > static_cast<unsigned>(DIAGONAL))
        throw std::out_of_range("Unexpected type of sudoku.");
    if (!s)
        throw std::runtime_error("Unexpected data in deserialized puzzle.");
    puzzle_type_ = static_cast<SudokuTypes>(type);
    SetupCheckers(size_, puzzle_type_);
}

} // namespace sudoku