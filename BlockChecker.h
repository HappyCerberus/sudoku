/* (c) 2020 RNDr. Simon Toth (happy.cerberus@gmail.com) */
#ifndef SUDOKU_BLOCKCHECKER_H
#define SUDOKU_BLOCKCHECKER_H

#include <vector>

class BlockChecker {
public:
    BlockChecker(const std::vector<int *> &elements);

    bool Check() const;

private:
    std::vector<int *> elem_;
};


#endif //SUDOKU_BLOCKCHECKER_H
