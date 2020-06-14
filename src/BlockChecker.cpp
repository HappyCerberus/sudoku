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
  for (unsigned i = 0; i < Size(); i++) {
    bool skip = false;
    for (unsigned j = 0; j < static_cast<unsigned>(whitelist.size()); j++) {
      if (i == whitelist[j])
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
void generic_recursive_find(
    std::vector<std::vector<unsigned>> &result, const std::vector<T *> &elems,
    unsigned size,
    const std::function<bool(const std::vector<T *> &,
                             const std::vector<unsigned> &)> &is_valid_set,
    unsigned consumed, std::vector<unsigned> path, unsigned first_index) {
  for (unsigned i = first_index; i < elems.size() - (size - consumed - 1);
       i++) {
    if ((size - consumed) == 1) {
      std::vector<unsigned> r = path;
      r.push_back(i);
      if (is_valid_set(elems, r)) {
        result.push_back(r);
      }
    } else {
      std::vector<unsigned> new_path = path;
      new_path.push_back(i);
      generic_recursive_find(result, elems, size, is_valid_set, consumed + 1,
                             new_path, i + 1);
    }
  }
}

template <typename T>
void generic_recursive_find(
    std::vector<std::vector<unsigned>> &result, const std::vector<T *> &elems,
    unsigned size,
    std::function<bool(const std::vector<T *> &, const std::vector<unsigned> &)>
        is_valid_set) {
  generic_recursive_find(result, elems, size, is_valid_set, 0,
                         std::vector<unsigned>{}, 0);
}

// if we pick X squares, and the number of possibilities within these squares
// is X -> then these possibilities can't appear anywhere else in the block
void recursive_set_find(std::vector<std::vector<unsigned>> &result,
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
  generic_recursive_find<sudoku::Square>(result, squares, size, check);
}

// if X possibilities only appear in X squares, then those squares can't
// contain any other numbers
void recursive_number_find(std::vector<std::vector<unsigned>> &result,
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
  generic_recursive_find<sudoku::Square>(result, squares, size, check);
}

// fish: grab X number of blocks, we look at the positions of a number, if
// the size of union of the positions == X, then we have a fish
void recursive_fish_find(std::vector<std::vector<unsigned>> &result,
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
  generic_recursive_find<BlockChecker>(result, blocks, size, check);
}

// finned fish: grab X number of blocks, we look at the positions of a
// number, if the size of union of the positions == X + 1, we check if there
// is a fin, as in a position that is only in one of the blocks
//
// because if we then remove the singular position we have a fish of size X
void recursive_finned_fish_find(std::vector<std::vector<unsigned>> &result,
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
  generic_recursive_find<BlockChecker>(result, blocks, size, check);
}

void BlockChecker::SolveNakedGroups() const {
  for (unsigned i = 2; i <= Size() / 2; i++) {
    SolveNakedGroups(i);
  }
}

void BlockChecker::SolveNakedGroups(unsigned size) const {
  std::vector<std::vector<unsigned>> result;
  recursive_set_find(result, elem_, size);
  for (size_t x = 0; x < result.size(); x++) {
    Square u(Size());
    u.ResetToEmpty();
    for (unsigned y = 0; y < size; y++) {
      u += *elem_[result[x][y]];
    }
    for (unsigned j = 0; j < Size(); j++) {
      bool skip = false;
      for (unsigned y = 0; y < size; y++)
        if (result[x][y] == j)
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
  std::vector<std::vector<unsigned>> result;
  recursive_number_find(result, elem_, size);
  for (size_t x = 0; x < result.size(); x++) {
    Square u(Size());
    u.ResetToEmpty();
    for (unsigned y = 0; y < size; y++) {
      u += result[x][y] + 1u;
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

} // namespace sudoku