/* (c) 2020 RNDr. Simon Toth (happy.cerberus@gmail.com) */

#ifndef SUDOKU_SMALLKILLERBLOCKCHECKER_H
#define SUDOKU_SMALLKILLERBLOCKCHECKER_H

// In the solver we want to get the killer blocks are fully contained within the SmallKillerBlockChecker, or that intersect with it.

#include "Square.h"

namespace sudoku {
class SmallKillerBlockChecker {
public:
    SmallKillerBlockChecker(unsigned sum, std::vector<Square*> squares) : sum_(sum), squares_(std::move(squares)) {}

    bool IsFullyContained(const std::vector<Square*>& squares) const;
    bool IsIntersecting(const std::vector<Square*>& squares) const;

    unsigned Sum() const { return sum_; }
    unsigned Size() const { return static_cast<unsigned>(squares_.size()); }

private:
    unsigned sum_;
    std::vector<Square*> squares_;
};

}

#endif //SUDOKU_SMALLKILLERBLOCKCHECKER_H
