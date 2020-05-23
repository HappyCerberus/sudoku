/* (c) 2020 RNDr. Simon Toth (happy.cerberus@gmail.com) */

#include "BlockChecker.h"
#include <bitset>

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
  for (size_t i = 0; i < elem_.size(); i++) {
    if (elem_[i]->IsSet())
      continue;
    elem_[i]->Remove(number);
  }
}

// if we pick X squares, and the number of possibilities within these squares
// is X -> then these possibilities can't appear anywhere else in the block
void recursive_set_find(std::vector<std::vector<size_t>> &result,
                        const std::vector<SquareType *> &squares, size_t size,
                        size_t consumed, std::vector<size_t> path,
                        size_t first_index) {
  for (size_t i = first_index; i < squares.size() - (size - consumed - 1);
       i++) {
    if ((size - consumed) == 1) {
      SquareType tmp_union(squares.size());
      tmp_union.SetToEmpty();
      for (size_t x : path) {
        tmp_union.Union(*squares[x]);
      }
      tmp_union.Union(*squares[i]);
      if (tmp_union.CountPossible() == size) {
        std::vector<size_t> r = path;
        r.push_back(i);
        result.push_back(r);
      }
    } else {
      std::vector<size_t> new_path = path;
      new_path.push_back(i);
      recursive_set_find(result, squares, size, consumed + 1, new_path, i + 1);
    }
  }
}

// if X possibilities only appear in X squares, then those squares can't
// contain any other numbers
void recursive_number_find(std::vector<std::vector<size_t>> &result,
                           const std::vector<SquareType *> &squares,
                           size_t size, size_t consumed,
                           std::vector<size_t> numbers, size_t first_index) {
  for (size_t i = first_index; i < squares.size() - (size - consumed - 1);
       i++) {
    if ((size - consumed) == 1) {
      SquareType tmp_numbers(squares.size());
      tmp_numbers.SetToEmpty();
      for (size_t x : numbers) {
        tmp_numbers.Add(x + 1);
      }
      tmp_numbers.Add(i + 1);

      size_t count_squares = 0;
      for (size_t x = 0; x < squares.size(); x++) {
        if (squares[x]->HasIntersection(tmp_numbers))
          count_squares++;
      }

      if (count_squares == size) {
        std::vector<size_t> r = numbers;
        r.push_back(i);
        result.push_back(r);
      }
    } else {
      std::vector<size_t> new_path = numbers;
      new_path.push_back(i);
      recursive_number_find(result, squares, size, consumed + 1, new_path,
                            i + 1);
    }
  }
}

void BlockChecker::Solve() {
  for (size_t i = 1; i <= elem_.size() - 1; i++) {
    std::vector<std::vector<size_t>> result;
    recursive_number_find(result, elem_, i);
    for (size_t x = 0; x < result.size(); x++) {
      SquareType u(elem_.size());
      u.SetToEmpty();
      for (size_t y = 0; y < result[x].size(); y++) {
        u.Add(result[x][y] + 1);
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
      SquareType u(elem_.size());
      u.SetToEmpty();
      for (size_t y = 0; y < result[x].size(); y++) {
        u.Union(*elem_[result[x][y]]);
      }
      for (size_t j = 0; j < elem_.size(); j++) {
        bool skip = false;
        for (size_t y = 0; y < result[x].size(); y++)
          if (result[x][y] == j)
            skip = true;
        if (!skip)
          elem_[j]->Minus(u);
      }
    }
  }
}
