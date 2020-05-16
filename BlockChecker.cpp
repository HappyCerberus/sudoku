/* (c) 2020 RNDr. Simon Toth (happy.cerberus@gmail.com) */

#include "BlockChecker.h"

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
        if (elem_[i]->IsSet()) continue;
        elem_[i]->Remove(number);
    }
}
