// (c) 2020 RNDr. Simon Toth (happy.cerberus@gmail.com)

#ifndef SUDOKU_KILLERSUMS_H
#define SUDOKU_KILLERSUMS_H

#include <array>
#include "../core/BitSet.h"

namespace {

inline constexpr unsigned number_of_sets(unsigned max, unsigned num_squares, unsigned sum) {
    unsigned result = 0;
    for (auto s : sudoku::BitSetSets(max, num_squares)) {
        unsigned tmp_sum = 0;
        for (auto b : sudoku::BitSetBits(&s)) {
            tmp_sum += b;
        }
        if (tmp_sum == sum)
            result++;
    }
    return result;
}

inline consteval unsigned total_number_of_sets(unsigned max) {
    unsigned result = 0;
    for (unsigned i = 2; i <= max; i++) {
        for (unsigned j = 3; j <= std::min(max * (max + 1) / 2, max * i); j++) {
            result += number_of_sets(max, i, j);
        }
    }
    return result;
}

template<unsigned Max>
inline consteval std::array<std::array<std::pair<unsigned, unsigned>, Max * (Max + 1) / 2>, Max - 1>
generate_offsets() {
    std::array<std::array<std::pair<unsigned, unsigned>, Max * (Max + 1) / 2>, Max - 1> result;
    unsigned offset = 0;
    for (unsigned i = 2; i <= Max; i++) {
        for (unsigned j = 3; j <= std::min(Max * (Max + 1) / 2, Max * i); j++) {
            unsigned len = number_of_sets(Max, i, j);
            result[i - 2][j - 3] = std::make_pair(offset, len);
            offset += len;
        }
    }
    return result;
}

template<unsigned Max>
inline consteval std::array<sudoku::BitSet, total_number_of_sets(Max)> generate_sets() {
    std::array<sudoku::BitSet, total_number_of_sets(Max)> result;
    unsigned offset = 0;
    for (unsigned i = 2; i <= Max; i++) {
        for (unsigned j = 3; j <= std::min(Max * (Max + 1) / 2, Max * i); j++) {
            for (auto s : sudoku::BitSetSets(Max, i)) {
                unsigned tmp_sum = 0;
                for (auto b : sudoku::BitSetBits(&s)) {
                    tmp_sum += b;
                }
                if (tmp_sum == j) {
                    result[offset] = s;
                    offset++;
                }
            }
        }
    }
    return result;
}

}

namespace sudoku {

template < unsigned Max >
struct SumGrid {
    // num_squares -> sum -> offset : length
    static constexpr std::array<std::array<std::pair<unsigned,unsigned>, Max*(Max+1)/2>,Max-1> offsets = generate_offsets<Max>();
    static constexpr std::array<sudoku::BitSet,total_number_of_sets(Max)> sets = generate_sets<Max>();
};

constexpr unsigned getNumberOfSets(unsigned max, unsigned num_squares, unsigned sum) {
    if (max == 9) {
        return SumGrid<9>::offsets[num_squares-2][sum-3].second;
    }
    return 0;
}

constexpr BitSet getSet(unsigned max, unsigned num_squares, unsigned sum, unsigned offset) {
   if (max == 9) {
       unsigned base_offset = SumGrid<9>::offsets[num_squares-2][sum-3].first;
       return SumGrid<9>::sets[base_offset + offset];
   }
   return BitSet::Empty(max);
}

}

#endif //SUDOKU_KILLERSUMS_H
