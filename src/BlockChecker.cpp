/* (c) 2020 RNDr. Simon Toth (happy.cerberus@gmail.com) */

#include "BlockChecker.h"
#include "Square.h"
#include <bitset>
#include <functional>

namespace sudoku {

BlockChecker::BlockChecker(const SudokuBlockType &elements) : elem_(elements) {}

bool BlockChecker::HasConflict() const {
  std::vector<bool> present(elem_.size(), false);
  for (unsigned i = 0; i < Size(); i++) {
    if (!elem_[i]->IsSet())
      continue;
    if (present[elem_[i]->Value() - 1]) {
      return true;
    }
    present[elem_[i]->Value() - 1] = true;
  }
  return false;
}

void BlockChecker::Prune(unsigned number) {
  Prune(number, std::vector<unsigned>{});
}

void BlockChecker::Prune(unsigned int number,
                         const std::vector<unsigned> &whitelist) {
  Prune(number,whitelist.begin(), whitelist.end());
}

void BlockChecker::Prune(unsigned int number,
                         std::vector<unsigned>::const_iterator begin,
                         std::vector<unsigned>::const_iterator end) {
  for (unsigned i = 0; i < Size(); i++) {
    bool skip = false;
    for (auto j = begin; j != end; j++) {
      if (i == *j)
        skip = true;
    }
    if (skip == true)
      continue;
    if (elem_[i]->IsSet())
      continue;
    (*elem_[i]) -= number;
  }
}

std::unordered_set<unsigned>
BlockChecker::NumberPositions(unsigned number) const {
  std::unordered_set<unsigned> positions;
  NumberPositions(number, positions);
  return positions;
}

void BlockChecker::NumberPositions(
    unsigned number, std::unordered_set<unsigned> &positions) const {
  for (unsigned i = 0; i < Size(); i++) {
    if (elem_[i]->IsPossible(number)) {
      positions.insert(i);
    }
  }
}

void BlockChecker::SolveIntersection(BlockChecker &r) const {
  std::unordered_set<unsigned> result; // TODO: maybe change to a square
  std::unordered_set<sudoku::Square *> intersection;
  for (auto i : elem_) {
    for (auto j : r.elem_) {
      if (i == j)
        intersection.insert(j);
    }
  }

  // calculate cardinality of the number for the entire block
  // calculate cardinality of the number for the intersection
  std::vector<unsigned> full_card(Size(), 0);
  std::vector<unsigned> inter_card(Size(), 0);

  for (auto i : elem_) {
    // For all numbers.
    for (unsigned j = 0; j < Size(); j++) {
      if (i->IsPossible(j + 1u)) {
        full_card[j]++;
      }
    }
  }

  for (auto i : intersection) {
    for (unsigned j = 0; j < Size(); j++) {
      if (i->IsPossible(j + 1u)) {
        inter_card[j]++;
      }
    }
  }

  for (unsigned i = 0; i < Size(); i++) {
    if (inter_card[i] == full_card[i]) {
      result.insert(i + 1);
    }
  }

  for (auto &e : r.elem_) {
    bool skip = false;
    for (auto &i : intersection) {
      if (i == e)
        skip = true;
    }
    if (skip)
      continue;

    for (unsigned number : result) {
      (*e) -= number;
    }
  }
}

template <typename T>
void generic_nonrecursive_find(
    std::vector<unsigned> &result, const std::vector<T *> &elems,
    unsigned size,
    const std::function<bool(const std::vector<T *> &,
                             const std::vector<unsigned> &)> &is_valid_set) {
  std::vector<unsigned> path;
  path.reserve(elems.size());
  path.push_back(0);

  if (size == 1 && is_valid_set(elems, path)) {
    for (size_t i = 0; i < size; i++)
      result.push_back(path[i]);
  }

  while (true) {
    if (path.size() == size) {
      if (path[size - 1] < elems.size() - 1) {
        ++path[size - 1];
        if (is_valid_set(elems, path)) {
          for (size_t i = 0; i < size; i++)
            result.push_back(path[i]);
        }
        continue;
      }
    } else {
      for (unsigned i = static_cast<unsigned>(path.size()); i < size; i++) {
        path.push_back(path[i - 1] + 1);
      }
      if (is_valid_set(elems, path)) {
        for (size_t i = 0; i < size; i++)
          result.push_back(path[i]);
      }
      continue;
    }

    // if (path.size() == size && path[size-1] == elemens.size()-1)
    while (!path.empty() &&
           path[path.size() - 1] == elems.size() - 1 - (size - path.size())) {
      path.pop_back();
    }
    if (path.empty())
      break;
    ++path[path.size() - 1];
  }
}

// if we pick X squares, and the number of possibilities within these squares
// is X -> then these possibilities can't appear anywhere else in the block
void recursive_set_find(std::vector<unsigned> &result,
                        const std::vector<sudoku::Square *> &squares,
                        unsigned size) {
  auto check = [](const std::vector<sudoku::Square *> &elems,
                  const std::vector<unsigned> &set) -> bool {
    sudoku::Square tmp_union(elems.size());
    tmp_union.ResetToEmpty();
    for (unsigned x : set) {
      tmp_union += *elems[x];
    }
    return tmp_union.CountPossible() == set.size();
  };
  generic_nonrecursive_find<sudoku::Square>(result, squares, size, check);
}


void recursive_set_find(std::vector<unsigned> &result,
                        const std::vector<sudoku::Square *> &squares,
                        const std::unordered_set<unsigned>& changed_squares,
                        unsigned size) {
  auto check = [&changed_squares, size](const std::vector<sudoku::Square *>
      &elems,
                  const std::vector<unsigned> &set) -> bool {
    bool found = false;
    for (unsigned x : set) {
      if (elems[x]->CountPossible() > size) return false;
      if (changed_squares.contains(x)) found = true;
    }
    if (!found) return false;

    sudoku::Square tmp_union(elems.size());
    tmp_union.ResetToEmpty();
    for (unsigned x : set) {
      tmp_union += *elems[x];
    }
    return tmp_union.CountPossible() == set.size();
  };
  generic_nonrecursive_find<sudoku::Square>(result, squares, size, check);
}

// if X possibilities only appear in X squares, then those squares can't
// contain any other numbers
void recursive_number_find(std::vector<unsigned> &result,
                           const std::vector<sudoku::Square *> &squares,
                           unsigned size) {
  auto check = [](const std::vector<sudoku::Square *> &elems,
                  const std::vector<unsigned> &set) -> bool {
    sudoku::Square tmp_numbers(elems.size());
    tmp_numbers.ResetToEmpty();
    for (unsigned x : set) {
      tmp_numbers += x + 1u;
    }

    size_t count_squares = 0;
    for (unsigned x = 0; x < elems.size(); x++) {
      if (elems[x]->HasIntersection(tmp_numbers))
        count_squares++;
    }

    return count_squares == set.size();
  };
  generic_nonrecursive_find<sudoku::Square>(result, squares, size, check);
}

void recursive_number_find_filtered(std::vector<unsigned> &result,
                           const std::vector<sudoku::Square *> &squares,
                           const std::unordered_set<unsigned>& numbers,
                           unsigned size) {
  auto check = [&numbers](const std::vector<sudoku::Square *> &elems,
                  const std::vector<unsigned> &set) -> bool {
    sudoku::Square tmp_numbers(elems.size());
    tmp_numbers.ResetToEmpty();
    bool found = false;
    for (unsigned x : set) {
      tmp_numbers += x + 1u;
      if (numbers.contains(x+1u)) found = true;
    }
    if (!found) return false;

    size_t count_squares = 0;
    for (unsigned x = 0; x < elems.size(); x++) {
      if (elems[x]->HasIntersection(tmp_numbers))
        count_squares++;
    }

    return count_squares == set.size();
  };
  generic_nonrecursive_find<sudoku::Square>(result, squares, size, check);
}

// fish: grab X number of blocks, we look at the positions of a number, if
// the size of union of the positions == X, then we have a fish
void recursive_fish_find(std::vector<unsigned> &result,
                              const std::vector<BlockChecker *> &blocks,
                              unsigned size, unsigned number) {
  auto check = [number](const std::vector<BlockChecker *> &elems,
                        const std::vector<unsigned> &set) -> bool {
    std::unordered_set<unsigned> positions;
    for (size_t v : set) {
      auto pos = elems[v]->NumberPositions(number);
      if (pos.size() < 2)
        return false;

      positions.insert(pos.begin(), pos.end());
    }
    return positions.size() == set.size();
  };
  generic_nonrecursive_find<BlockChecker>(result, blocks, size, check);
}

// finned fish: grab X number of blocks, we look at the positions of a
// number, if the size of union of the positions == X + 1, we check if there
// is a fin, as in a position that is only in one of the blocks
//
// because if we then remove the singular position we have a fish of size X
void recursive_finned_fish_find(std::vector<unsigned> &result,
                         const std::vector<BlockChecker *> &blocks,
                         unsigned size, unsigned number) {
  auto check = [number](const std::vector<BlockChecker *> &elems,
                        const std::vector<unsigned> &set) -> bool {
    std::unordered_set<unsigned> positions;
    for (unsigned v : set) {
      auto pos = elems[v]->NumberPositions(number);
      if (pos.size() < 2)
        return false;

      positions.insert(pos.begin(), pos.end());
    }
    if (positions.size() == set.size() + 1) {
      for (unsigned p : positions) {
        unsigned count = 0;
        for (unsigned v : set) {
          if (elems[v]->HasNumberAtPosition(number, p))
            count++;
        }
        if (count == 1) {
          return true;
        }
      }
    }
    return false;
  };
  generic_nonrecursive_find<BlockChecker>(result, blocks, size, check);
}

void BlockChecker::SolveNakedGroups() const {
  for (unsigned i = 2; i <= Size() / 2; i++) {
    SolveNakedGroups(i);
  }
}

void BlockChecker::SolveNakedGroups(unsigned size) const {
  std::vector<unsigned> result;
  recursive_set_find(result, elem_, size);
  for (size_t x = 0; x < result.size()/size; x++) {
    Square u(Size());
    u.ResetToEmpty();
    for (unsigned y = 0; y < size; y++) {
      u += *elem_[result[x*size+y]];
    }
    for (unsigned j = 0; j < Size(); j++) {
      bool skip = false;
      for (unsigned y = 0; y < size; y++)
        if (result[x*size+y] == j)
          skip = true;
      if (!skip)
        (*elem_[j]) -= u;
    }
  }
}
void BlockChecker::SolveHiddenGroups() const {
  for (unsigned i = 1; i < Size() / 2; i++) {
    SolveHiddenGroups(i);
  }
}
void BlockChecker::SolveHiddenGroups(unsigned size) const {
  std::vector<unsigned> result;
  recursive_number_find(result, elem_, size);
  for (size_t x = 0; x < result.size()/size; x++) {
    Square u(Size());
    u.ResetToEmpty();
    for (unsigned y = 0; y < size; y++) {
      u += result[x*size+y] + 1u;
    }
    for (unsigned j = 0; j < Size(); j++) {
      if (elem_[j]->HasIntersection(u)) {
        (*elem_[j]) &= u;
      }
    }
  }
}
bool BlockChecker::HasNumberAtPosition(unsigned number, unsigned position)
    const {
  return elem_[position]->IsPossible(number);
}

std::unordered_set<unsigned> BlockChecker::RemovedNumbers(const std::vector<Square>&
state, const Square *base_pointer) const {
  std::unordered_set<unsigned> result;
  for (auto i : elem_) {
    size_t offset = static_cast<size_t>(i-base_pointer);
    Square diff = state[offset] - *i;
    unsigned next = 0;
    while ((next = diff.Next(next)) != 0) {
      result.insert(next);
    }
  }
  return result;
}

void BlockChecker::SolveHiddenGroups(unsigned size, const
                                     std::unordered_set<unsigned>&
removed_numbers) const {
  std::vector<unsigned> result;
  recursive_number_find_filtered(result, elem_, removed_numbers, size);
  for (size_t x = 0; x < result.size()/size; x++) {
    Square u(Size());
    u.ResetToEmpty();
    for (unsigned y = 0; y < size; y++) {
      u += result[x*size+y] + 1u;
    }
    for (unsigned j = 0; j < Size(); j++) {
      if (elem_[j]->HasIntersection(u)) {
        (*elem_[j]) &= u;
      }
    }
  }
}

std::unordered_set<unsigned> BlockChecker::ChangedSquares(const std::vector<Square>&
state, const Square *base_pointer) const {
  std::unordered_set<unsigned> result;
  for (size_t i = 0; i < elem_.size(); i++) {
    size_t offset = static_cast<size_t>(elem_[i]-base_pointer);
    if (state[offset] != *elem_[i]) {
      result.insert(i);
    }
  }
  return result;
}

void BlockChecker::SolveNakedGroups(unsigned size,
                                    const std::unordered_set<unsigned>&
changed_squares) const {
  std::vector<unsigned> result;
  recursive_set_find(result, elem_, changed_squares, size);
  for (size_t x = 0; x < result.size()/size; x++) {
    Square u(Size());
    u.ResetToEmpty();
    for (unsigned y = 0; y < size; y++) {
      u += *elem_[result[x*size+y]];
    }
    for (unsigned j = 0; j < Size(); j++) {
      bool skip = false;
      for (unsigned y = 0; y < size; y++)
        if (result[x*size+y] == j)
          skip = true;
      if (!skip)
        (*elem_[j]) -= u;
    }
  }
}

} // namespace sudoku