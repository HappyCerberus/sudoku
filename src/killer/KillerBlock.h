// (c) 2020 RNDr. Simon Toth (happy.cerberus@gmail.com)

#ifndef SUDOKU_KILLERBLOCK_H
#define SUDOKU_KILLERBLOCK_H

#include <vector>
#include "../core/BitSet.h"
#include "../core/UniqueBlock.h"
#include "KillerSums.h"
#include "../core/SudokuAlgorithms.h"

namespace sudoku {

class KillerBlock {
public:
    KillerBlock(std::vector<BitSet *> elem, unsigned max, unsigned sum) :
        block_(elem, max),
        possible_sets_(getNumberOfSets(max, static_cast<unsigned>(elem.size()), sum), true),
        sum_(sum) {}

    //! Return whether this block should contain all the numbers in the range.
    [[nodiscard]] bool IsCompleteBlock() const noexcept {
        return block_.IsCompleteBlock();
    }

    //! Return a bitset representing the positions of a number inside of this block.
    [[nodiscard]] BitSet NumberPositions(unsigned number) const noexcept {
        return block_.NumberPositions(number);
    }

    //! Remove the specified number from all squares in the block, except for the ones specified in the skip mask.
    void Prune(unsigned number, BitSet skip) noexcept {
        block_.Prune(number, skip);
    }

    //! Determine whether there is a number conflict in this block.
    [[nodiscard]] bool HasConflict() const noexcept {
        return block_.HasConflict();
    }

    //! Return the squares contained within this block.
    [[nodiscard]] const std::vector<BitSet *> &GetSquares() const noexcept {
        return block_.GetSquares();
    }

    //! Return the number of elements in this block
    [[nodiscard]] unsigned Size() const noexcept {
        return block_.Size();
    }

    //! Return the maximum of the number range for this block.
    [[nodiscard]] constexpr unsigned Max() const noexcept {
        return block_.Max();
    }

    //! Return the sum for this killer block.
    [[nodiscard]] constexpr unsigned Sum() const noexcept {
        return sum_;
    }

    //! Return the union of all the still possible sets for this killer block.
    [[nodiscard]] BitSet UnionSumSet() const noexcept {
        BitSet result;
        for (unsigned i = 0; i < possible_sets_.size(); i++) {
            if (possible_sets_[i])
                result |= getSet(block_.Max(), block_.Size(), sum_, i);
        }
        return result;
    }

    //! Remove sums that not possible given the state of the squares contained within the block.
    void PruneSumSetsBySquare() noexcept {
        for (auto &s : block_.GetSquares()) {
            for (unsigned i = 0; i < possible_sets_.size(); i++) {
                if (possible_sets_[i] && (getSet(block_.Max(), block_.Size(), sum_, i) & (*s)).CountSet() == 0) {
                    possible_sets_[i] = false;
                }
            }
        }
    }

    //! Remove sums that not possible given the state of the squares contained within the block.
    void PruneSumSets() noexcept {
        BitSet set = BitSet::Empty(Max());
        for (auto &s : block_.GetSquares()) {
            set |= *s;
        }
        for (unsigned i = 0; i < possible_sets_.size(); i++) {
            if (possible_sets_[i] &&
                getSet(block_.Max(), block_.Size(), sum_, i).HasAdditionalBits(set)) {
              possible_sets_[i] = false;
            }
        }
    }

private:
    UniqueBlock block_;
    std::vector<bool> possible_sets_;
    unsigned sum_;

    friend std::vector<bool> &TestGetPossibleSets(KillerBlock &s);
};

}

#endif //SUDOKU_KILLERBLOCK_H
