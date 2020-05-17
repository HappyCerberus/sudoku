// (c) 2020 RNDr. Simon Toth (happy.cerberus@gmail.com)

#ifndef SUDOKU_SUDOKUSQUARE_H
#define SUDOKU_SUDOKUSQUARE_H

#include <vector>
#include <cstdint>

template<typename T>
class SudokuSquare {
public:
    SudokuSquare(unsigned max_value = 9) : max_(max_value), data_(0) {
        // 0000 0000
        // 1111 1111
        // 1111 0000
        // 0000 1111
        data_ = ~((~0u) << max_value);
    }

    bool IsPossible(unsigned number) const {
        return (data_ & (1 << (number - 1))) != 0;
    }

    // C++ 20 has popcount
    unsigned CountPossible() const {
        unsigned result = 0;
        for (unsigned i = 1; i <= max_; i++) {
            if (IsPossible(i))
                result++;
        }
        return result;
    }

    bool IsSet() const {
        return CountPossible() == 1;
    }

    unsigned Value() const {
        if (CountPossible() != 1) return 0;
        for (unsigned i = 1; i <= max_; i++) {
            if (IsPossible(i)) return i;
        }
        // unreachable
        return 0;
    }

    unsigned Next(unsigned number) const {
        for (unsigned i = number + 1; i <= max_; i++) {
            if (IsPossible(i)) return i;
        }
        return 0;
    }

    void Reset() {
        data_ = ~((~0u) << max_);
    }

    void Set(unsigned number) {
        data_ = (1 << (number - 1));
    }

    void Remove(unsigned number) {
        data_ &= ~(1 << (number - 1));
    }

private:
    unsigned max_;
    T data_;
};

typedef SudokuSquare<uint16_t> SquareType;

#endif //SUDOKU_SUDOKUSQUARE_H
