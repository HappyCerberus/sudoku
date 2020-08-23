/* (c) 2020 RNDr. Simon Toth (happy.cerberus@gmail.com) */

#include "SmallKillerBlockChecker.h"

namespace sudoku {

bool SmallKillerBlockChecker::IsFullyContained(const std::vector<Square *> &squares) const {
    for (const BitSet* square : squares) {
        bool found = false;
        for (const BitSet* s : squares_) {
            if (s == square) found = true;
        }
        if (!found) return false;
    }
    return true;
}

bool SmallKillerBlockChecker::IsIntersecting(const std::vector<Square *> &squares) const {
    for (const BitSet* square : squares) {
        bool found = false;
        for (const BitSet* s : squares_) {
            if (s == square) found = true;
        }
        if (found) return true;
    }
    return false;
}


}