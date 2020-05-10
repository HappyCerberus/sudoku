/* (c) 2020 RNDr. Simon Toth (happy.cerberus@gmail.com) */
#ifndef SUDOKU_SUDOKU_H
#define SUDOKU_SUDOKU_H

#include "BlockChecker.h"
#include <iosfwd>

enum SudokuTypes {
    BASIC = 1,
    DIAGONAL = 2
};

class Sudoku {
public:
    Sudoku(unsigned size = 9, SudokuTypes type = BASIC);

    Sudoku(std::vector<std::vector<int>> data, SudokuTypes type = BASIC);

    bool CheckPuzzle();

    std::vector<std::vector<int>> &data() { return data_; }

    const std::vector<std::vector<int>> &data() const { return data_; }


private:
    std::vector<BlockChecker> checks_;
    std::vector<std::vector<int>> data_;

    void SetupCheckers(unsigned size = 9, SudokuTypes type = BASIC);

};

std::ostream &operator<<(std::ostream &s, const Sudoku &puzzle);

#endif //SUDOKU_SUDOKU_H
