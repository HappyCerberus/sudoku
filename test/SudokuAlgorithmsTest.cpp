#include <catch2/catch.hpp>
#include "../src/core/SudokuAlgorithms.h"
#include "../src/core/BitSet.h"

namespace sudoku {

TEST_CASE("Sudoku Algorithms : Union Squares Test", "[union]") {
    BitSet a = BitSet::SingleBit(9,1) | BitSet::SingleBit(9,3) | BitSet::SingleBit(9,5);
    std::vector<BitSet> values;
    values.reserve(9);
    std::vector<BitSet*> pointers;
    for (unsigned i = 9; i >= 1; i--) {
        values.push_back(BitSet::SingleBit(9,i));
        pointers.push_back(&values.back());
    }
    BitSet b = Union(pointers, a);
    REQUIRE(b.CountSet() == 3);
    REQUIRE(b.IsBitSet(9));
    REQUIRE(b.IsBitSet(7));
    REQUIRE(b.IsBitSet(5));
}

TEST_CASE("Sudoku Algorithms : SolveNakedGroups 1", "[naked_groups]") {
    std::vector<BitSet> values{9, BitSet::SudokuSquare(9)};
    values[3] = BitSet::SingleBit(9u, 1u);
    std::vector<BitSet*> pointers;
    for (auto &b : values) {
        pointers.push_back(&b);
    }

    SolveNakedGroups(pointers, 1);
    REQUIRE(values[0].CountSet() == 8);
    REQUIRE(values[1].CountSet() == 8);
    REQUIRE(values[2].CountSet() == 8);
    REQUIRE(values[3].CountSet() == 1);
    REQUIRE(values[4].CountSet() == 8);
    REQUIRE(values[5].CountSet() == 8);
    REQUIRE(values[6].CountSet() == 8);
    REQUIRE(values[7].CountSet() == 8);
    REQUIRE(values[8].CountSet() == 8);
    REQUIRE(values[3].IsBitSet(1));
    REQUIRE(!values[0].IsBitSet(1));
}

TEST_CASE("Sudoku Algorithms : SolveNakedGroups 3", "[naked_groups]") {
    std::vector<BitSet> values{9, BitSet::SudokuSquare(9)};
    values[1] = BitSet::SingleBit(9u, 2u);
    values[5] = BitSet::SingleBit(9u, 9u);
    values[7] = BitSet::SingleBit(9u, 5u);
    std::vector<BitSet*> pointers;
    for (auto &b : values) {
        pointers.push_back(&b);
    }

    SolveNakedGroups(pointers, 1);
    REQUIRE(values[0].CountSet() == 6);
    REQUIRE(values[1].CountSet() == 1);
    REQUIRE(values[2].CountSet() == 6);
    REQUIRE(values[3].CountSet() == 6);
    REQUIRE(values[4].CountSet() == 6);
    REQUIRE(values[5].CountSet() == 1);
    REQUIRE(values[6].CountSet() == 6);
    REQUIRE(values[7].CountSet() == 1);
    REQUIRE(values[8].CountSet() == 6);
    REQUIRE(values[1].IsBitSet(2));
    REQUIRE(values[5].IsBitSet(9));
    REQUIRE(values[7].IsBitSet(5));
    REQUIRE(!values[4].IsBitSet(2));
    REQUIRE(!values[4].IsBitSet(9));
    REQUIRE(!values[4].IsBitSet(5));
}

TEST_CASE("Sudoku Algorithms : SolveNakedGroups 2", "[naked_groups]") {
    std::vector<BitSet> values{9, BitSet::SudokuSquare(9)};
    BitSet a = BitSet::SingleBit(9,2) | BitSet::SingleBit(9,3) | BitSet::SingleBit(9,7);
    values[1] &= a;
    values[4] &= a;
    values[8] &= a;
    std::vector<BitSet*> pointers;
    for (auto &b : values) {
        pointers.push_back(&b);
    }

    SolveNakedGroups(pointers, 3);
    REQUIRE(values[0].CountSet() == 6);
    REQUIRE(values[1].CountSet() == 3);
    REQUIRE(values[2].CountSet() == 6);
    REQUIRE(values[3].CountSet() == 6);
    REQUIRE(values[4].CountSet() == 3);
    REQUIRE(values[5].CountSet() == 6);
    REQUIRE(values[6].CountSet() == 6);
    REQUIRE(values[7].CountSet() == 6);
    REQUIRE(values[8].CountSet() == 3);
    REQUIRE(values[0].IsBitSet(1));
    REQUIRE(!values[0].IsBitSet(2));
}

TEST_CASE("Sudoku Algorithms : SolveHiddenGroups 1", "[hidden_groups]") {
    std::vector<BitSet> values{9, BitSet::SudokuSquare(9)};
    for (auto &v : values) {
        v -= 1;
    }
    values[3] += 1;

    std::vector<BitSet*> pointers;
    for (auto &b : values) {
        pointers.push_back(&b);
    }

    SolveHiddenGroups(pointers, 1);
    REQUIRE(values[0].CountSet() == 8);
    REQUIRE(values[1].CountSet() == 8);
    REQUIRE(values[2].CountSet() == 8);
    REQUIRE(values[3].CountSet() == 1);
    REQUIRE(values[4].CountSet() == 8);
    REQUIRE(values[5].CountSet() == 8);
    REQUIRE(values[6].CountSet() == 8);
    REQUIRE(values[7].CountSet() == 8);
    REQUIRE(values[8].CountSet() == 8);
    REQUIRE(values[3].IsBitSet(1));
    REQUIRE(!values[0].IsBitSet(1));
}

TEST_CASE("Sudoku Algorithms : SolveHiddenGroups X", "[hidden_groups]") {
    std::vector<BitSet> values{9, BitSet::SudokuSquare(9)};
    for (auto &v : values) {
        v -= 1;
    }
    values[3] += 1;
    values[4] = BitSet::SingleBit(9u,1u);

    std::vector<BitSet*> pointers;
    for (auto &b : values) {
        pointers.push_back(&b);
    }

    SolveHiddenGroups(pointers, 1);
    REQUIRE(values[0].CountSet() == 8);
    REQUIRE(values[1].CountSet() == 8);
    REQUIRE(values[2].CountSet() == 8);
    REQUIRE(values[3].CountSet() == 9);
    REQUIRE(values[4].CountSet() == 1);
    REQUIRE(values[5].CountSet() == 8);
    REQUIRE(values[6].CountSet() == 8);
    REQUIRE(values[7].CountSet() == 8);
    REQUIRE(values[8].CountSet() == 8);
    REQUIRE(values[3].IsBitSet(1));
    REQUIRE(!values[0].IsBitSet(1));
    REQUIRE(values[4].IsBitSet(1));
}

TEST_CASE("Sudoku Algorithms : SolveHiddenGroups 1b", "[hidden_groups]") {
    std::vector<BitSet> values{9, BitSet::SudokuSquare(9)};
    for (auto &v : values) {
        v -= 1;
    }
    values[3] += 1;
    values[4] += 1;

    std::vector<BitSet*> pointers;
    for (auto &b : values) {
        pointers.push_back(&b);
    }

    SolveHiddenGroups(pointers, 1);
    REQUIRE(values[0].CountSet() == 8);
    REQUIRE(values[1].CountSet() == 8);
    REQUIRE(values[2].CountSet() == 8);
    REQUIRE(values[3].CountSet() == 9);
    REQUIRE(values[4].CountSet() == 9);
    REQUIRE(values[5].CountSet() == 8);
    REQUIRE(values[6].CountSet() == 8);
    REQUIRE(values[7].CountSet() == 8);
    REQUIRE(values[8].CountSet() == 8);
    REQUIRE(values[3].IsBitSet(1));
    REQUIRE(values[4].IsBitSet(1));
}

TEST_CASE("Sudoku Algorithms : SolveHiddenGroups 2", "[hidden_groups]") {
    std::vector<BitSet> values{9, BitSet::SudokuSquare(9)};
    for (auto &v : values) {
        v -= 4;
        v -= 6;
        v -= 7;
    }
    values[1] += 4;
    values[1] += 6;
    values[2] += 4;
    values[2] += 7;
    values[5] += 6;
    values[5] += 7;

    std::vector<BitSet*> pointers;
    for (auto &b : values) {
        pointers.push_back(&b);
    }

    SolveHiddenGroups(pointers, 3);
    REQUIRE(values[0].CountSet() == 6);
    REQUIRE(values[1].CountSet() == 2);
    REQUIRE(values[2].CountSet() == 2);
    REQUIRE(values[3].CountSet() == 6);
    REQUIRE(values[4].CountSet() == 6);
    REQUIRE(values[5].CountSet() == 2);
    REQUIRE(values[6].CountSet() == 6);
    REQUIRE(values[7].CountSet() == 6);
    REQUIRE(values[8].CountSet() == 6);
    REQUIRE((values[1].IsBitSet(4) && values[1].IsBitSet(6)));
    REQUIRE((values[2].IsBitSet(4) && values[2].IsBitSet(7)));
    REQUIRE((values[5].IsBitSet(6) && values[5].IsBitSet(7)));
}

namespace {
struct MiniTestPuzzle {
    MiniTestPuzzle() : data(9*9, BitSet::SudokuSquare(9))  {
        blocks.reserve(27);
    for (unsigned i = 0; i < 9; i++) {
        std::vector<BitSet*> row;
        for (unsigned j = 0; j < 9; j++) {
            row.push_back(&data[i*9+j]);
        }
        blocks.emplace_back(std::move(row), 9u);
        rows.push_back(&blocks.back());
    }   
    for (unsigned j = 0; j < 9; j++) {
        std::vector<BitSet*> col;
        for (unsigned i = 0; i < 9; i++) {
            col.push_back(&data[i*9+j]);
        }
        blocks.emplace_back(std::move(col), 9u);
        cols.push_back(&blocks.back());
    }
    for (unsigned i = 0; i < 3; i++) {
      for (unsigned j = 0; j < 3; j++) {
        std::vector<BitSet*> square;
        for (unsigned x = i*3; x < (i+1)*3; x++) {
          for (unsigned y = j*3; y < (j+1)*3; y++) {
            square.push_back(&data[x*9+y]);
          }
        }
        blocks.emplace_back(std::move(square), 9u);
        squares.push_back(&blocks.back());
      }
    }
    }

    std::vector<BitSet> data;
    std::vector<UniqueBlock> blocks;
    std::vector<UniqueBlock*> rows;
    std::vector<UniqueBlock*> cols;
    std::vector<UniqueBlock*> squares;
};
}

TEST_CASE("Sudoku Algorithms : SolveFish X-Wing 1", "[fish]") {
    MiniTestPuzzle puzzle;


  for (unsigned i = 0; i < 9; i++) {
    for (unsigned j = 0; j < 9; j++) {
      puzzle.data[i*9+j] -= 4;
    }
  }
/*
[.23......] [........9] [.23......] [...456...] [1........] [....56...] [......7..] [.......8.] [...45....]
[1........] [.......8.] [.....6...] [.2.......] [...4..7..] [....5.7..] [...45....] [..3......] [........9]
[....5....] [...4.....] [......7..] [........9] [.......8.] [..3......] [.....6...] [.2.......] [1........]
[.23......] [1........] [....5....] [.......8.] [.....6...] [...4.....] [.23......] [........9] [......7..]
[.....6...] [.23...7..] [...4.....] [..3.5.7..] [.23...7.9] [....5.7.9] [.......8.] [1........] [.23.5....]
[........9] [.23...7..] [.......8.] [..3.5.7..] [.23...7..] [1........] [.23.5....] [...4.....] [.....6...]
[......7..] [.23......] [1........] [..34.6...] [..34....9] [.....6..9] [.234.....] [....5....] [.......8.]
[.......8.] [....5....] [........9] [..34..7..] [..3...7..] [.2.......] [1........] [.....6...] [..34.....]
[...4.....] [.....6...] [.23......] [1........] [....5....] [.......8.] [........9] [......7..] [.23......]
 */


  puzzle.data[0*9+3] += 4;
  puzzle.data[0*9+8] += 4;
  puzzle.data[1*9+4] += 4;
  puzzle.data[1*9+6] += 4;
  puzzle.data[2*9+1] += 4;
  puzzle.data[3*9+5] += 4;
  puzzle.data[4*9+2] += 4;
  puzzle.data[5*9+7] += 4;
  puzzle.data[6*9+3] += 4;
  puzzle.data[6*9+4] += 4;
  puzzle.data[6*9+6] += 4;
  puzzle.data[7*9+3] += 4;
  puzzle.data[7*9+8] += 4;
  puzzle.data[8*9+0] += 4;

  SolveFish(puzzle.cols, puzzle.rows, 2, 4);

  unsigned count = 0;
  for (unsigned i = 0; i < 9; i++) {
    for (unsigned j = 0; j < 9; j++) {
      if (puzzle.data[i*9+j].IsBitSet(4)) count++;
    }
  }

  REQUIRE(!puzzle.data[6*9+3].IsBitSet(4));
  REQUIRE(count == 13);
}

TEST_CASE("Sudoku Algorithms : SolveFish X-Wing 2", "[fish]") {
    MiniTestPuzzle puzzle;
  for (unsigned j = 0; j < 9; j++) {
    if (j == 2 || j == 6) continue;
    puzzle.data[2*9+j] -= 1;
    puzzle.data[5*9+j] -= 1;
  }

  SolveFish(puzzle.rows, puzzle.cols, 2, 1);
  for (unsigned i = 0; i < 9; i++) {
    for (unsigned j = 0; j < 9; j++) {
      if (i == 2 || i == 5) {
        REQUIRE(puzzle.data[i*9+j].IsBitSet(1) == (j == 2 || j == 6));
      } else {
        REQUIRE(puzzle.data[i*9+j].IsBitSet(1) != (j == 2 || j == 6));
      }
    }
  }
}

TEST_CASE("Sudoku Algorithms : SolveFish X-Wing 3", "[fish]") {
    MiniTestPuzzle puzzle;
  for (unsigned i = 0; i < 9; i++) {
    for (unsigned j = 0; j < 9; j++) {
      puzzle.data[i*9+j] -= 4;
    }
  }

  puzzle.data[6*9+3] += 4;
  puzzle.data[6*9+4] += 4;
  puzzle.data[6*9+6] += 4;
  puzzle.data[1*9+4] += 4;
  puzzle.data[1*9+6] += 4;


  SolveFish(puzzle.cols, puzzle.rows, 2, 4);
  unsigned count = 0;
  for (unsigned i = 0; i < 9; i++) {
    for (unsigned j = 0; j < 9; j++) {
      if (puzzle.data[i*9+j].IsBitSet(4)) count++;
    }
  }

  REQUIRE(!puzzle.data[6*9+3].IsBitSet(4));
  REQUIRE(count == 4);
}

TEST_CASE("Sudoku Algorithms : SolveFinnedFish 1", "[finned]") {
    MiniTestPuzzle test;
    MiniTestPuzzle expect;

    for (unsigned j = 0; j < 9; j++) {
      if (j == 2 || j == 6)
        continue;
      test.data[2*9+j] -= 1;
      test.data[5*9+j] -= 1;
      expect.data[2*9+j] -= 1;
      expect.data[5*9+j] -= 1;
    }

    test.data[5*9+0] += 1;
    expect.data[5*9+0] += 1;
    expect.data[3*9+2] -= 1;
    expect.data[4*9+2] -= 1;

    SolveFinnedFish(test.rows, [&test](unsigned number, unsigned row, unsigned col, BitSet rows, BitSet cols){
        REQUIRE(number == 1);
        REQUIRE(row == 6);
        REQUIRE(col == 1);
        REQUIRE(rows.CountSet() == 2);
        REQUIRE(rows.IsBitSet(3));
        REQUIRE(rows.IsBitSet(6));
        REQUIRE(cols.CountSet() == 3);
        REQUIRE(cols.IsBitSet(1));
        REQUIRE(cols.IsBitSet(3));
        REQUIRE(cols.IsBitSet(7));
        test.data[3*9+(3-1)] -= 1;
        test.data[4*9+(3-1)] -= 1;
    },2, 1);

    for (unsigned i = 0; i < 9; i++) {
      for (unsigned j = 0; j < 9; j++) {
        INFO(i << j);
        REQUIRE(test.data[i*9+j] == expect.data[i*9+j]);
      }
    }
  }

TEST_CASE("Sudoku Algorithms : SolveFinnedFish 2", "[finned]") {
  MiniTestPuzzle test;
  MiniTestPuzzle expect;
    
  for (unsigned j = 0; j < 9; j++) {
    if (j == 2 || j == 6)
      continue;
    test.data[j*9+2] -= 1;
    test.data[j*9+5] -= 1;
    expect.data[j*9+2] -= 1;
    expect.data[j*9+5] -= 1;
  }

  test.data[0*9+5] += 1;
  expect.data[0*9+5] += 1;
  expect.data[2*9+3] -= 1;
  expect.data[2*9+4] -= 1;

  SolveFinnedFish(test.cols, [&test](unsigned number, unsigned col, unsigned row, BitSet cols, BitSet rows){
      REQUIRE(number == 1);
      REQUIRE(row == 1);
      REQUIRE(col == 6);
      REQUIRE(cols.CountSet() == 2);
      REQUIRE(cols.IsBitSet(3));
      REQUIRE(cols.IsBitSet(6));
      REQUIRE(rows.CountSet() == 3);
      REQUIRE(rows.IsBitSet(1));
      REQUIRE(rows.IsBitSet(3));
      REQUIRE(rows.IsBitSet(7));
      test.data[2*9+3] -= 1;
      test.data[2*9+4] -= 1;
  }, 2, 1);

  for (unsigned i = 0; i < 9; i++) {
    for (unsigned j = 0; j < 9; j++) {
    
      REQUIRE(test.data[i*9+j] == expect.data[i*9+j]);
    }
  }
}

TEST_CASE("Sudoku Algorithms : SolveFinnedFish 3", "[finned]") {
  MiniTestPuzzle test;
  MiniTestPuzzle expect;
    
  for (unsigned j = 0; j < 9; j++) {
    if (j == 2 || j == 4 || j == 6 )
      continue;
    test.data[j*9+2] -= 1;
    test.data[j*9+5] -= 1;
    expect.data[j*9+2] -= 1;
    expect.data[j*9+5] -= 1;
  }

  SolveFinnedFish(test.cols, [&test](unsigned, unsigned, unsigned, BitSet, BitSet){
    CHECK(false);
  }, 2, 1);

  for (unsigned i = 0; i < 9; i++) {
    for (unsigned j = 0; j < 9; j++) {
      CHECK(test.data[i*9+j] == expect.data[i*9+j]);
    }
  }
}

TEST_CASE("Sudoku Algorithms : SolveFinnedFish 4", "[finned]") {
  MiniTestPuzzle test;
  MiniTestPuzzle expect;
    
  for (unsigned j = 0; j < 9; j++) {
    if (j == 2 || j == 6)
      continue;
    test.data[j*9+2] -= 1;
    test.data[j*9+5] -= 1;
    expect.data[j*9+2] -= 1;
    expect.data[j*9+5] -= 1;
  }

  test.data[3*9+5] += 1;
  expect.data[3*9+5] += 1;

  SolveFinnedFish(test.cols, [&test](unsigned number, unsigned col, unsigned row, BitSet cols, BitSet rows){
      REQUIRE(number == 1);
      REQUIRE(row == 4);
      REQUIRE(col == 6);
      REQUIRE(cols.CountSet() == 2);
      REQUIRE(cols.IsBitSet(3));
      REQUIRE(cols.IsBitSet(6));
      REQUIRE(rows.CountSet() == 3);
      REQUIRE(rows.IsBitSet(3));
      REQUIRE(rows.IsBitSet(4));
      REQUIRE(rows.IsBitSet(7));
  }, 2, 1);

  for (unsigned i = 0; i < 9; i++) {
    for (unsigned j = 0; j < 9; j++) {
      CHECK(test.data[i*9+j] == expect.data[i*9+j]);
    }
  }
}

TEST_CASE("Sudoku Algorithms : SolveIntersection 1", "[intersection]") {
  MiniTestPuzzle test;
  MiniTestPuzzle expect;

  for (unsigned j = 0; j < 9; j++) {
    if (j != 2) {
      (*test.rows[1]->GetSquares()[j]) -= 1;
      (*expect.rows[1]->GetSquares()[j]) -= 1;
    }
    if (j != 1) {
      (*expect.cols[2]->GetSquares()[j]) -= 1;
    }
  }

  SolveBlockIntersection(*test.rows[1], *test.cols[2]);
  REQUIRE(test.data[1*9+2].IsBitSet(1));

  for (unsigned i = 0; i < 9; i++) {
    for (unsigned j = 0; j < 9; j++) {
      INFO(i << " " << j);
      REQUIRE(test.data[i*9+j] == expect.data[i*9+j]);
    }
  }
}

TEST_CASE("Sudoku Algorithms : SolveIntersection 2", "[intersection]") {
  MiniTestPuzzle test;
  MiniTestPuzzle expect;

  for (unsigned j = 0; j < 9; j++) {
    if (j < 3 || j >= 6) {
      (*test.cols[5]->GetSquares()[j]) -= 1;
      (*expect.cols[5]->GetSquares()[j]) -= 1;
    } else {
      (*expect.cols[3]->GetSquares()[j]) -= 1;
      (*expect.cols[4]->GetSquares()[j]) -= 1;
    }
  }

  SolveBlockIntersection(*test.cols[5], *test.squares[4]);

  for (unsigned i = 0; i < 9; i++) {
    for (unsigned j = 0; j < 9; j++) {
      INFO(i << " " << j);
      REQUIRE(test.data[i*9+j] == expect.data[i*9+j]);
    }
  }

}

}