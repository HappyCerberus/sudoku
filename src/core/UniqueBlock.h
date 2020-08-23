// (c) 2020 RNDr. Simon Toth (happy.cerberus@gmail.com)

#ifndef CORE_UNIQUE_BLOCK_H_
#define CORE_UNIQUE_BLOCK_H_

#include <vector>
#include "BitSet.h"

namespace sudoku {

class UniqueBlock {
public:
    UniqueBlock(std::vector<BitSet*> elem, unsigned max) : elem_(std::move(elem)), max_(max) {}

    //! Return whether this block should contain all the numbers in the range.
    [[nodiscard]] bool IsCompleteBlock() const noexcept { 
        return max_ == elem_.size(); 
    }

    //! Return a bitset representing the positions of a number inside of this block.
    [[nodiscard]] BitSet NumberPositions(unsigned number) const noexcept {
        BitSet result = BitSet::Empty(max_);
        for (unsigned i = 0; i < elem_.size(); i++) {
            if (elem_[i]->IsBitSet(number)) {
                result += (i+1);
            }
        }
        return result;
    }

    //! Remove the specified number from all squares in the block, except for the ones specified in the skip mask.
    void Prune(unsigned number, BitSet skip) noexcept {
        for (unsigned i = 0; i < elem_.size(); i++) {
            if (skip.IsBitSet(i+1))
                continue;
            (*elem_[i]) -= number;
        }
    }

    //! Determine whether there is a number conflict in this block.
    [[nodiscard]] bool HasConflict() const noexcept {
        for (unsigned i = 1; i <= Max(); i++) {
            bool found = false;
            for (auto &e : elem_) {
                if (e->HasSingletonValue() && e->SingletonValue() == i) {
                    if (found)
                        return true;
                    else
                        found = true;
                }
            }
        }
        return false;
    }

    //! Return the squares contained within this block.
    [[nodiscard]] const std::vector<BitSet*>& GetSquares() const noexcept {
        return elem_;
    }

    //! Return the number of elements in this block
    [[nodiscard]] unsigned Size() const noexcept {
        return static_cast<unsigned>(elem_.size());
    }

    //! Return the maximum of the number range for this block.
    [[nodiscard]] constexpr unsigned Max() const noexcept {
        return max_;
    }

private:
    std::vector<BitSet*> elem_;
    unsigned max_;

    // Test friends.
    friend std::vector<::sudoku::BitSet *> &TestGetBlockData(UniqueBlock &s);
};

}

#endif // CORE_UNIQUE_BLOCK_H_