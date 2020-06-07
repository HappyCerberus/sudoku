// (c) 2020 RNDr. Simon Toth (happy.cerberus@gmail.com)

#include "Square.h"

#include <ostream>
#include <sstream>

namespace sudoku {

std::string Square::DebugString() const {
  std::stringstream s;
  s << "[";
  for (unsigned i = 0; i < max_; i++) {
    if (IsPossible(i + 1)) {
      if (i + 1 > 9) {
        s << static_cast<char>(i - 9 + 'A');
      } else {
        s << i + 1;
      }
    } else {
      s << ".";
    }
  }
  s << "]";
  return s.str();
}

std::ostream &operator<<(std::ostream &s, const Square &r) {
  s << "[";
  for (unsigned i = 0; i < r.max_; i++) {
    if (r.IsPossible(i + 1)) {
      if (i + 1 > 9) {
        s << static_cast<char>(i - 9 + 'A');
      } else {
        s << i + 1;
      }
    } else {
      s << ".";
    }
  }
  s << "]";
  return s;
}

} // namespace sudoku