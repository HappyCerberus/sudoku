/* (c) 2020 RNDr. Simon Toth (happy.cerberus@gmail.com) */

#include "Sudoku.h"
#include "core/BitSet.h"
#include "core/SudokuAlgorithms.h"
#include <functional>
#include <iomanip>
#include <iostream>
#include <ostream>
#include <iterator>
#include <map>
#include <sstream>
#include <unordered_map>

const std::map<unsigned, unsigned> block_sums = {
        {9, 45},
        {16, 136},
        {25, 325},
        {36, 666},
        {49, 1225},
        {64, 2080},
};

namespace sudoku {
Sudoku::Sudoku(unsigned size, SudokuTypes type)
    : data_(size*size, sudoku::BitSet::SudokuSquare(size)),
      block_mapping_(size, std::vector<std::vector<UniqueBlock *>>(
                               size, std::vector<UniqueBlock *>())),
      size_(size), solution_(nullptr), puzzle_type_(type) {
  SetupCheckers(size, type);
  MakeCopy();
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
    std::vector<::sudoku::BitSet *> row;
    for (size_t j = 0; j < size; j++) {
      row.push_back(&data_[i*Size()+j]);
    }
    checks_.emplace_back(row, Size());
    for (size_t j = 0; j < size; j++) {
      block_mapping_[i][j].push_back(&checks_[checks_.size() - 1]);
    }
    row_checks_.push_back(&checks_[checks_.size() - 1]);
  }

  for (size_t j = 0; j < size; j++) {
    std::vector<::sudoku::BitSet *> column;
    for (size_t i = 0; i < size; i++) {
      column.push_back(&data_[i*Size()+j]);
    }
    checks_.emplace_back(column, Size());
    for (size_t i = 0; i < size; i++) {
      block_mapping_[i][j].push_back(&checks_[checks_.size() - 1]);
    }
    col_checks_.push_back(&checks_[checks_.size() - 1]);
  }

  std::unordered_map<unsigned, unsigned> blocksizes = {{9, 3}, {16, 4}};
  unsigned bsize = blocksizes[size];
  for (size_t i = 0; i < bsize; i++) {
    for (size_t j = 0; j < bsize; j++) {
      std::vector<::sudoku::BitSet *> block;
      for (size_t x = i * bsize; x < (i + 1) * bsize; x++) {
        for (size_t y = j * bsize; y < (j + 1) * bsize; y++) {
          block.push_back(&data_[x*Size()+y]);
        }
      }
      checks_.emplace_back(block, Size());
      for (size_t x = i * bsize; x < (i + 1) * bsize; x++) {
        for (size_t y = j * bsize; y < (j + 1) * bsize; y++) {
          block_mapping_[x][y].push_back(&checks_[checks_.size() - 1]);
        }
      }
    }
  }

  if (type == DIAGONAL) {
      std::vector<::sudoku::BitSet *> d1, d2;
      for (size_t i = 0; i < size; i++) {
          d1.push_back(&data_[i * Size() + i]);
          d2.push_back(&data_[i * Size() + size - 1 - i]);
      }
      checks_.emplace_back(d1, Size());
      checks_.emplace_back(d2, Size());
      for (size_t i = 0; i < size; i++) {
          block_mapping_[i][i].push_back(&checks_[checks_.size() - 2]);
          block_mapping_[i][size - 1 - i].push_back(&checks_[checks_.size() - 1]);
      }
  }
}

void Sudoku::DebugPrint(std::ostream &s) {
  for (unsigned i = 0; i < Size(); i++) {
    for (unsigned j = 0; j < Size(); j++) {
      s << data_[i*Size()+j].DebugString() << " ";
    }
    s << std::endl;
  }
}
std::string Sudoku::DebugString() {
  std::stringstream s;
  for (unsigned i = 0; i < Size(); i++) {
    for (unsigned j = 0; j < Size(); j++) {
      s << data_[i*Size()+j].DebugString() << " ";
    }
    s << std::endl;
  }
  return s.str();
}


bool Sudoku::HasChange() const {
  for (unsigned i = 0; i < data_.size(); i++) {
    if (data_[i] != data_copy_[i])
      return true;
  }
  return false;
}

void Sudoku::ResetChange() {
  MakeCopy();
}

std::unordered_set<UniqueBlock *> Sudoku::ChangedBlocks() const {
  std::unordered_set<UniqueBlock *> result;
  for (unsigned i = 0; i < data_.size(); i++) {
    if (data_[i] != data_copy_[i]) {
      for (auto &b : block_mapping_[i/Size()][i%Size()]) {
        result.insert(b);
      }
    }
  }
  return result;
}

bool Sudoku::IsSet() const {
  for (unsigned i = 0; i < Size(); i++) {
    for (unsigned j = 0; j < Size(); j++) {
      if (!data_[i*Size()+j].HasSingletonValue())
        return false;
    }
  }
  return true;
}

void Sudoku::SolveFish(unsigned int size, unsigned int number) {
  ::sudoku::SolveFish(GetRowBlocks(), GetColBlocks(), size, number);
  ::sudoku::SolveFish(GetColBlocks(), GetRowBlocks(), size, number);
}

void Sudoku::SolveFinnedFish(unsigned int size, unsigned int number) {
  ::sudoku::SolveFinnedFish(GetRowBlocks(), [this](unsigned num, 
    unsigned row, unsigned col, 
    BitSet rows, BitSet cols) {
      for (auto block : block_mapping_[row-1][col-1]) {
        for (auto square : block->GetSquares()) {
          unsigned r = static_cast<unsigned>((square-data_.data())/Size()) + 1;
          unsigned c = static_cast<unsigned>((square-data_.data())%Size()) + 1;
          // skip over squares that share the column or row with the fin
          if (row == r || col == c) continue;

          bool found = false;
          for (auto bit : BitSetBits(&cols)) {
            // on the same column as the fish
            if (bit == c) {
              found = true;
              break;
            }
          }
          if (!found) continue;

          found = false;
          for (auto bit : BitSetBits(&rows)) {
            // on the same row as the fish
            if (bit == r) {
              found = true;
              break;
            }
          }
          if (found) continue;

          (*square) -= num;
        }
      }
  }, size, number);
  ::sudoku::SolveFinnedFish(GetColBlocks(), [this](unsigned num, 
    unsigned col, unsigned row, 
    BitSet cols, BitSet rows) {
      for (auto block : block_mapping_[row-1][col-1]) {
        for (auto square : block->GetSquares()) {
          unsigned r = static_cast<unsigned>((square-data_.data())/Size()) + 1;
          unsigned c = static_cast<unsigned>((square-data_.data())%Size()) + 1;
          // skip over squares that share the column or row with the fin
          if (row == r || col == c) continue;

          bool found = false;
          for (auto bit : BitSetBits(&rows)) {
            // on the same row as the fish
            if (bit == r) {
              found = true;
              break;
            }
          }
          if (!found) continue;

          found = false;
          for (auto bit : BitSetBits(&cols)) {
            // on the same column as the fish
            if (bit == c) {
              found = true;
              break;
            }
          }
          if (found) continue;

          (*square) -= num;
        }
      }
  }, size, number);
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
      if (data_[i*Size()+j].IsBitSet(number)) {
        result.nodes.push_back(i*Size()+j);
      }
    }
  }
  for (const auto& b : checks_) {
    auto pos = b.NumberPositions(number);
    if (pos.CountSet() == 2) {
      auto i = BitSetBits(&pos).begin();
      BitSet* first = b.GetSquares()[(*i)-1];
      i++;
      BitSet* second = b.GetSquares()[(*i)-1];

      result.strong_links.insert({first-&data_[0], second-&data_[0]});
      result.weak_links.insert({first-&data_[0], second-&data_[0]});
      result.strong_links.insert({second-&data_[0], first-&data_[0]});
      result.weak_links.insert({second-&data_[0], first-&data_[0]});
    } else if (pos.CountSet() > 2) {
      for (auto i : BitSetBits(&pos)) {
        for (auto j : BitSetBits(&pos)) {
          if (i == j) continue;

          BitSet* first = b.GetSquares()[i-1];
          BitSet* second = b.GetSquares()[j-1];
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
      if (!puzzle[i][j].HasSingletonValue()) {
        s << std::setw(2) << 0;
        continue;
      }
        if (puzzle[i][j].SingletonValue() >= 10) {
          s << std::setw(2) << static_cast<char>(puzzle[i][j].SingletonValue() - 10 + 'A');
        } else {
          s << std::setw(2) << puzzle[i][j].SingletonValue();
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
        puzzle[i][j] = BitSet::SudokuSquare(puzzle.Size());
        continue;
      }
      if (isalpha(c))
        puzzle[i][j] = BitSet::SingleBit(puzzle.Size(), static_cast<unsigned>(c - 'A') + 10u);
      if (isdigit(c))
        puzzle[i][j] = BitSet::SingleBit(puzzle.Size(), static_cast<unsigned>(c - '0'));
    }
  }
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

    if (data_[i->second].CountSet() != 2) continue;

    // what is the other number in the square != source_number
    unsigned next = 0;
    for (auto it : BitSetBits(&data_[i->second])) {
      if (it != next_number) {
        next = it;
        break;
      }
    }
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
    if (data_[n].CountSet() == 2) {
      path.push_back(n);
      // what is the other number in the square != source_number
      unsigned next = 0;
      for (auto it : BitSetBits(&data_[n])) {
        if (it != number) {
          next = it;
          break;
        }
      }
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

  std::unordered_set<BitSet *> bs;
  for (auto s : bb) {
    for (unsigned i = 0; i < Size(); i++) {
      bool skip = false;
      for (auto j : path) {
        if (&data_[j] == s->GetSquares()[i]) skip = true;
      }
      if (skip) continue;
      bs.insert(s->GetSquares()[i]);
    }
  }

  for (auto s : be) {
    for (unsigned i = 0; i < Size(); i++) {
      bool skip = false;
      for (auto j : path) {
        if (&data_[j] == s->GetSquares()[i]) skip = true;
      }
      if (skip) continue;
      if (bs.find(s->GetSquares()[i]) != bs.end()) {
        (*s->GetSquares()[i]) -= number;
        modified = true;
      }
    }
  }

  return modified;
}

void Sudoku::PruneKillerBlockSums() {
    for (auto& killer : killers_) {
        killer.PruneSumSets();
        killer.PruneSumSetsBySquare();
    }
}

void Sudoku::PruneSquaresFromKillerBlocks() {
    for (auto& killer : killers_) {
        BitSet u = killer.UnionSumSet();
        for (auto& square : killer.GetSquares()) {
            (*square) &= u;
        }
    }
}

void Sudoku::PreBuildKillerMapping() {
    for (unsigned killerId = 0; killerId < killers_.size(); killerId++) {
        auto &killer = killers_[killerId];
        std::vector<UniqueBlock *> intersection = GetBlockMapping(killer.GetSquares()[0]);
        for (unsigned i = 1; i < killer.GetSquares().size(); i++) {
            std::vector<UniqueBlock *> current = GetBlockMapping(killer.GetSquares()[i]);
            std::vector<UniqueBlock *> new_inter;
            std::set_intersection(intersection.begin(), intersection.end(),
                                  current.begin(), current.end(),
                                  std::back_inserter(new_inter));
            intersection.swap(new_inter);
        }

        for (auto &block : intersection) {
            unsigned offset = static_cast<unsigned>(block - &checks_[0]);
            contained_killer_blocks_.insert(std::make_pair(offset, killerId));
        }
    }
}

void Sudoku::ProcessContainedKillerBlocks(unsigned blockId, unsigned &num_squares, unsigned &killer_sum, BitSet &found) {
    num_squares = 0;
    killer_sum = 0;
    found = BitSet::SudokuSquare(checks_[blockId].Size());
    auto range = contained_killer_blocks_.equal_range(blockId);
    for (auto it = range.first; it != range.second; it++) {
        num_squares += killers_[it->second].Size();
        killer_sum += killers_[it->second].Sum();
        // go over all the squares within the killer block
        for (auto &square : killers_[it->second].GetSquares()) {
            for (unsigned i = 0; i < checks_[blockId].Size(); i++) {
                if (square == checks_[blockId].GetSquares()[i]) {
                    found -= (i+1);
                }
            }
        }
    }
}

void Sudoku::AddKillerSingles() {
    for (unsigned blockId = 0; blockId < checks_.size(); blockId++) {
        unsigned num_squares = 0;
        unsigned killer_sum = 0;
        BitSet found = BitSet::SudokuSquare(checks_[blockId].Size());
        ProcessContainedKillerBlocks(blockId, num_squares, killer_sum, found);
        if (num_squares == checks_[blockId].Size() - 1) {
            assert(found.HasSingletonValue());
            auto square = checks_[blockId].GetSquares()[found.SingletonValue()-1];
            (*square) = BitSet::SingleBit(checks_[blockId].Max(), block_sums.at(checks_[blockId].Size()) - killer_sum);
        }
    }
}

void Sudoku::AddKillerRemainders(unsigned size) {
    for (unsigned blockId = 0; blockId < checks_.size(); blockId++) {
        unsigned num_squares = 0;
        unsigned killer_sum = 0;
        BitSet found = BitSet::SudokuSquare(checks_[blockId].Size());
        ProcessContainedKillerBlocks(blockId, num_squares, killer_sum, found);
        if (num_squares == checks_[blockId].Size() - size) {
            std::vector<BitSet*> squares;
            for (auto bit : BitSetBits(&found)) {
                squares.push_back(checks_[blockId].GetSquares()[bit-1]);
            }
            killers_.emplace_back(std::move(squares), checks_[blockId].Max(), block_sums.at(checks_[blockId].Size()) - killer_sum);
        }
    }
}

void Sudoku::SerializeKillerBlock(const KillerBlock& k, std::ostream& s) const {
    s << 0 << ":"; // killer block tag
    s << k.Size() << ":" << k.Sum() << ":";
    for (auto& square : k.GetSquares()) {
        unsigned offset = static_cast<unsigned>(square-&data_[0]);
        s << offset << ":";
    }
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
    for (auto &killer : killers_) {
        SerializeKillerBlock(killer, s);
    }
    return s.str();
}

void Sudoku::DeserializeKillerBlock(std::istream& s) {
    char delim = 0;
    unsigned number_of_squares, sum;
    s >> number_of_squares >> delim >> sum >> delim;
    if (!s)
        throw std::runtime_error("Unexpected data in deserialized killer block.");
    std::vector<BitSet*> squares;
    for (unsigned i = 0; i < number_of_squares; i++) {
        unsigned offset;
        s >> offset >> delim;
        if (!s)
            throw std::runtime_error("Unexpected offset data in deserialized killer block.");
        squares.push_back(&data_[offset]);
    }

    killers_.emplace_back(std::move(squares), Max(), sum);
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
    data_ = std::vector<BitSet>(size_*size_, BitSet{size_});
    block_mapping_ = std::vector<std::vector<std::vector<UniqueBlock *>>>(size_, std::vector<std::vector<UniqueBlock *>>(size_, std::vector<UniqueBlock *>()));
    checks_.clear();
    row_checks_.clear();
    col_checks_.clear();

    for (unsigned i = 0; i < Size(); i++) {
        for (unsigned j = 0; j < Size(); j++) {
            data_[i*Size()+j].Deserialize(s, poss);
            s >> delim;
            if (!s)
                throw std::runtime_error("Unexpected data in deserialized puzzle.");
        }
    }

    unsigned type = 0;
    s >> type >> delim;
    if (!s)
        throw std::runtime_error("Unexpected data in deserialized puzzle.");
    if (type > static_cast<unsigned>(DIAGONAL))
        throw std::out_of_range("Unexpected type of sudoku.");
    puzzle_type_ = static_cast<SudokuTypes>(type);
    SetupCheckers(size_, puzzle_type_);

    // Read additional blocks.
    unsigned block_type = 0;
    while (s >> block_type >> delim) {
        // Killer block
        if (block_type == 0) {
            DeserializeKillerBlock(s);
        }
    }
}

} // namespace sudoku
