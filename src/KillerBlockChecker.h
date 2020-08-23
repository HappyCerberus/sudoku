/* (c) 2020 RNDr. Simon Toth (happy.cerberus@gmail.com) */

#ifndef SUDOKU_KILLERBLOCKCHECKER_H
#define SUDOKU_KILLERBLOCKCHECKER_H

#include <vector>
#include "core/BitSet.h"

namespace sudoku {
class BlockChecker;

// std::map size -> sum -> vector -> vector number
// size[puzzle] -> size -> sum -> vector<vector<unsinged>>

class KillerBlockChecker {
public:
    KillerBlockChecker(unsigned sum, std::vector<BitSet*> squares) : sum_(sum), squares_(std::move(squares)) {}

    unsigned Size() const { return static_cast<unsigned>(squares_.size()); }
    unsigned Sum() const { return sum_; }

    /*! Find numbers that are fully contained within an intersection with another
     *  block, and remove those from non-intersection squares of the other block.
     *
     *  @param r Other block to intersect with and prune.
     */
    void SolveIntersection(BlockChecker &r) const;

    const std::vector<Square*> GetSquares() const;

private:
    unsigned sum_;
    std::vector<BitSet*> squares_;
};

}

#endif //SUDOKU_KILLERBLOCKCHECKER_H
