/* (c) 2020 RNDr. Simon Toth (happy.cerberus@gmail.com) */

#include "BlockChecker.h"
#include "Square.h"
#include <bitset>
#include <functional>

BlockChecker::BlockChecker(const SudokuBlockType &elements) : elem_(elements) {}

bool BlockChecker::Check() const {
  std::vector<bool> present(elem_.size(), false);
  for (size_t i = 0; i < elem_.size(); i++) {
    if (!elem_[i]->IsSet())
      continue;
    if (present[elem_[i]->Value() - 1]) {
      return false;
    }
    present[elem_[i]->Value() - 1] = true;
  }
  return true;
}

void BlockChecker::Prune(unsigned int number) {
  Prune(number, std::vector<size_t>{});
}

template <typename T>
void generic_recursive_find(
    std::vector<std::vector<size_t>> &result, const std::vector<T *> &elems,
    size_t size,
    const std::function<bool(const std::vector<T *> &,
                             const std::vector<size_t> &)> &is_valid_set,
    size_t consumed, std::vector<size_t> path, size_t first_index) {
  for (size_t i = first_index; i < elems.size() - (size - consumed - 1); i++) {
    if ((size - consumed) == 1) {
      std::vector<size_t> r = path;
      r.push_back(i);
      if (is_valid_set(elems, r)) {
        result.push_back(r);
      }
    } else {
      std::vector<size_t> new_path = path;
      new_path.push_back(i);
      generic_recursive_find(result, elems, size, is_valid_set, consumed + 1,
                             new_path, i + 1);
    }
  }
}

template <typename T>
void generic_recursive_find(
    std::vector<std::vector<size_t>> &result, const std::vector<T *> &elems,
    size_t size,
    std::function<bool(const std::vector<T *> &, const std::vector<size_t> &)>
        is_valid_set) {
  generic_recursive_find(result, elems, size, is_valid_set, 0,
                         std::vector<size_t>{}, 0);
}

// if we pick X squares, and the number of possibilities within these squares
// is X -> then these possibilities can't appear anywhere else in the block
void recursive_set_find(std::vector<std::vector<size_t>> &result,
                        const std::vector<sudoku::SquareType *> &squares, size_t size) {
  auto check = [](const std::vector<sudoku::SquareType *> &elems,
                  const std::vector<size_t> &set) -> bool {
    sudoku::SquareType tmp_union(elems.size());
    tmp_union.SetToEmpty();
    for (size_t x : set) {
      tmp_union += *elems[x];
    }
    return tmp_union.CountPossible() == set.size();
  };
  generic_recursive_find<sudoku::SquareType>(result, squares, size, check);
}

// if X possibilities only appear in X squares, then those squares can't
// contain any other numbers
void recursive_number_find(std::vector<std::vector<size_t>> &result,
                           const std::vector<sudoku::SquareType *> &squares,
                           size_t size) {
  auto check = [](const std::vector<sudoku::SquareType *> &elems,
                  const std::vector<size_t> &set) -> bool {
    sudoku::SquareType tmp_numbers(elems.size());
    tmp_numbers.SetToEmpty();
    for (size_t x : set) {
      tmp_numbers += x + 1;
    }

    size_t count_squares = 0;
    for (size_t x = 0; x < elems.size(); x++) {
      if (elems[x]->HasIntersection(tmp_numbers))
        count_squares++;
    }

    return count_squares == set.size();
  };
  generic_recursive_find<sudoku::SquareType>(result, squares, size, check);
}

// swordfish: grab X number of blocks, we look at the positions of a number, if
// the size of union of the positions == X, then we have a swordfish
void recursive_swordfish_find(std::vector<std::vector<size_t>> &result,
                              const std::vector<BlockChecker *> &blocks,
                              size_t size, size_t number) {
  auto check = [number](const std::vector<BlockChecker *> &elems,
                        const std::vector<size_t> &set) -> bool {
    std::unordered_set<size_t> positions;
    for (size_t v : set) {
      elems[v]->NumberPositions(number, positions);
    }
    return positions.size() == set.size();
  };
  generic_recursive_find<BlockChecker>(result, blocks, size, check);
}

void BlockChecker::Solve() {
  for (size_t i = 1; i <= elem_.size() - 1; i++) {
    std::vector<std::vector<size_t>> result;
    recursive_number_find(result, elem_, i);
    for (size_t x = 0; x < result.size(); x++) {
      sudoku::SquareType u(elem_.size());
      u.SetToEmpty();
      for (size_t y = 0; y < result[x].size(); y++) {
        u += result[x][y] + 1;
      }
      for (size_t j = 0; j < elem_.size(); j++) {
        if (elem_[j]->HasIntersection(u)) {
          elem_[j]->Intersect(u);
        }
      }
    }
  }

  for (size_t i = 2; i <= elem_.size() - 1; i++) {
    std::vector<std::vector<size_t>> result;
    recursive_set_find(result, elem_, i);
    for (size_t x = 0; x < result.size(); x++) {
      sudoku::SquareType u(elem_.size());
      u.SetToEmpty();
      for (size_t y = 0; y < result[x].size(); y++) {
        u += *elem_[result[x][y]];
      }
      for (size_t j = 0; j < elem_.size(); j++) {
        bool skip = false;
        for (size_t y = 0; y < result[x].size(); y++)
          if (result[x][y] == j)
            skip = true;
        if (!skip)
          (*elem_[j]) -= u;
      }
    }
  }
}

void BlockChecker::PruneInterection(BlockChecker &r) const {
  std::unordered_set<size_t> result;
  std::unordered_set<sudoku::SquareType *> intersection;
  for (auto i : elem_) {
    for (auto j : r.elem_) {
      if (i == j)
        intersection.insert(j);
    }
  }

  // calculate cardinality of the number for the entire block
  // calculate cardinality of the number for the intersection
  std::vector<size_t> full_card(elem_.size(), 0);
  std::vector<size_t> inter_card(elem_.size(), 0);

  for (auto i : elem_) {
    for (size_t j = 0; j < elem_.size(); j++) {
      if (i->IsPossible(j + 1)) {
        full_card[j]++;
      }
    }
  }

  for (auto i : intersection) {
    for (size_t j = 0; j < elem_.size(); j++) {
      if (i->IsPossible(j + 1)) {
        inter_card[j]++;
      }
    }
  }

  for (size_t i = 0; i < elem_.size(); i++) {
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

    for (size_t number : result) {
      (*e) -= number;
    }
  }
}
std::unordered_set<size_t> BlockChecker::NumberPositions(size_t number) const {
  std::unordered_set<size_t> positions;
  for (size_t i = 0; i < elem_.size(); i++) {
    if (elem_[i]->IsPossible(number)) {
      positions.insert(i);
    }
  }
  return positions;
}
void BlockChecker::NumberPositions(
    size_t number, std::unordered_set<size_t> &positions) const {
  for (size_t i = 0; i < elem_.size(); i++) {
    if (elem_[i]->IsPossible(number)) {
      positions.insert(i);
    }
  }
}
void BlockChecker::Prune(unsigned int number,
                         const std::vector<size_t> &whitelist) {
  for (size_t i = 0; i < elem_.size(); i++) {
    bool skip = false;
    for (size_t j = 0; j < whitelist.size(); j++) {
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
