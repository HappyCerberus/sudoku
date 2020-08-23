/* (c) 2020 RNDr. Simon Toth (happy.cerberus@gmail.com) */

#include "KillerBlockChecker.h"

#include <unordered_set>

#include "BlockChecker.h"

namespace sudoku {

const std::vector<Square *> sudoku::KillerBlockChecker::GetSquares() const {
    return squares_;
}

void KillerBlockChecker::SolveIntersection(BlockChecker &r) const {
    std::unordered_set<unsigned> result; // TODO: maybe change to a square
    std::unordered_set<sudoku::Square *> intersection;
    for (auto i : squares_) {
        for (auto j : r.elem_) {
            if (i == j)
                intersection.insert(j);
        }
    }

    // calculate cardinality of the number for the entire block
    // calculate cardinality of the number for the intersection
    std::vector<unsigned> full_card(Size(), 0);
    std::vector<unsigned> inter_card(Size(), 0);

    for (auto i : squares_) {
        // For all numbers.
        for (unsigned j = 0; j < Size(); j++) {
            if (i->IsPossible(j + 1u)) {
                full_card[j]++;
            }
        }
    }

    for (auto i : intersection) {
        for (unsigned j = 0; j < Size(); j++) {
            if (i->IsPossible(j + 1u)) {
                inter_card[j]++;
            }
        }
    }

    for (unsigned i = 0; i < Size(); i++) {
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

        for (unsigned number : result) {
            (*e) -= number;
        }
    }
}

}