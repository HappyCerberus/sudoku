// (c) 2020 RNDr. Simon Toth (happy.cerberus@gmail.com)

#ifndef CORE_SUDOKU_ALGORITHMS_H_
#define CORE_SUDOKU_ALGORITHMS_H_

#include "BitSet.h"
#include "UniqueBlock.h"

#include <vector>
#include <functional>

namespace sudoku {

inline BitSet Union(const std::vector<BitSet*> &squares, BitSet selector) {
    BitSet result = BitSet::Empty(static_cast<unsigned>(squares.size()));
    for (auto iter : BitSetBits(&selector)) {
      result |= *squares[iter-1];
    }
    return result;
}

inline BitSet NumberPositions(const std::vector<BitSet*> &squares, unsigned number) {
    BitSet result = BitSet::Empty(static_cast<unsigned>(squares.size()));
    for (unsigned i = 0; i < squares.size(); i++) {
        if (squares[i]->IsBitSet(number))
            result += (i+1);
    }
    return result;
}

inline void SolveNakedGroups(const std::vector<BitSet*> &squares, unsigned size) {
    const unsigned num_elem = static_cast<unsigned>(squares.size());
    for (auto iter : BitSetSets(num_elem, size)) {
        BitSet u = Union(squares, iter);
        if (u.CountSet() == size) {
            for (BitSet* s : squares) {
                if (s->HasAdditionalBits(u)) {
                    (*s) -= u;
                }
            }
        }
    }
}

inline void SolveHiddenGroups(const std::vector<BitSet*> &squares, unsigned size) {
    const unsigned num_elem = static_cast<unsigned>(squares.size());
    for (auto iter : BitSetSets(num_elem, size)) {
        unsigned count = 0;
        for (auto &s : squares) {
            if (s->HasSingletonValue()) {
              if (s->HasIntersection(iter)) {
                count = 0;
                break;
              }
              continue;
            }
            if (s->HasIntersection(iter))
                count++;
        }
        if (count == size) {
            for (BitSet* s : squares) {
                if (s->HasIntersection(iter)) {
                    (*s) &= iter;
                }
            }
        }
    }
}

inline void SolveFish(const std::vector<UniqueBlock *> &blocks, const std::vector<UniqueBlock*> &orthogonal, unsigned size, unsigned number) {
    const unsigned num_elem = static_cast<unsigned>(blocks.size());
    for (auto iter : BitSetSets(num_elem, size)) {
      bool valid = true;
      BitSet set = BitSet::Empty(num_elem);
      for (auto bit : BitSetBits(&iter)) {
        BitSet x = blocks[bit-1]->NumberPositions(number);
        if (x.CountSet() < 2) {
          valid = false;
          break;
        }
        set |= x;
      }

      if (!valid || set.CountSet() != size)
        continue;

      for (auto bit : BitSetBits(&set)) {
        orthogonal[bit-1]->Prune(number, iter);
      }
    }
}

inline void SolveFinnedFish(const std::vector<UniqueBlock *> &blocks, 
                            const std::function<void(unsigned, unsigned, unsigned, BitSet, BitSet)> &prune,
                            unsigned size, unsigned number) {
    const unsigned num_elem = static_cast<unsigned>(blocks.size());
    for (auto iter : BitSetSets(num_elem, size)) {
      bool valid = true;
      BitSet set = BitSet::Empty(num_elem);
      for (auto bit : BitSetBits(&iter)) {
        BitSet x = blocks[bit-1]->NumberPositions(number);
        if (x.CountSet() < 2) {
          valid = false;
          break;
        }
        set |= x;
      }

      if (!valid || set.CountSet() != size + 1)
        continue;

      // We know that we have something like finned fish, we need to check if it actually has a fin.
      unsigned block_id = 0;
      unsigned orthogonal_id = 0;
      valid = false;
      for (auto i : BitSetBits(&set)) {
        unsigned count = 0;
        for (auto j : BitSetBits(&iter)) {
          if (blocks[j-1]->NumberPositions(number).IsBitSet(i)) {
            block_id = j;
            count++;
          }
        }
        if (count == 1) {
          orthogonal_id = i;
          valid = true;
          break;
        }
      }

      // We know that the fin is at [block_id, orthogonal_id], set is the list of possitions for all the numbers.
      if (valid) {
        prune(number, block_id, orthogonal_id, iter, set);
      }
    }
}

inline void SolveBlockIntersection(const UniqueBlock& forcing_block, UniqueBlock& checked) {
  BitSet forced_numbers = BitSet::Empty(forcing_block.Max());
  for (unsigned k = 1; k <= forcing_block.Max(); k++) {
    unsigned count = 0;
    for (auto i : forcing_block.GetSquares()) {
      if (i->IsBitSet(k))
        count++;

      for (auto j : checked.GetSquares()) {
        if (i != j) continue;
        if (i->IsBitSet(k))
          count--;
      }
    }
    if (count == 0) {
      forced_numbers += k;
    }
  }

  for (auto j : checked.GetSquares()) {
    bool found = false;
    for (auto i : forcing_block.GetSquares()) {
      if (i == j)
        found = true;
    }
    if (!found)
      (*j) -= forced_numbers;
  }
}



}

#endif // CORE_SUDOKU_ALGORITHMS_H_