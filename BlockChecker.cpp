/* (c) 2020 RNDr. Simon Toth (happy.cerberus@gmail.com) */

#include "BlockChecker.h"

BlockChecker::BlockChecker(const std::vector<int *> &elements) : elem_(elements) {}

bool BlockChecker::Check() const {
    std::vector<bool> present(elem_.size(), false);
    for (int i = 0; i < elem_.size(); i++) {
        if (*(elem_[i]) == 0)
            continue;
        if (present[*(elem_[i]) - 1]) {
            return false;
        }
        present[*(elem_[i]) - 1] = true;
    }
    return true;
}