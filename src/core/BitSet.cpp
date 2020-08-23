// (c) 2020 RNDr. Simon Toth (happy.cerberus@gmail.com)

#include "BitSet.h"

#include <iostream>
#include <sstream>

namespace sudoku {

void BitSet::Serialize(std::ostream &s) const {
  s << data_;
}

void BitSet::Deserialize(std::istream &s, unsigned max) {
  s >> data_;
  max_ = max;
}

std::string BitSet::DebugString() const {
  std::stringstream s;
  s << "[";
  for (unsigned i = 0; i < max_; i++) {
    if (IsBitSet(i + 1)) {
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

std::ostream& operator << (std::ostream& s, const BitSet& b) {
  s << b.DebugString();
  return s;
}

}