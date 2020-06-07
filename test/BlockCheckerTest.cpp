/* (c) 2020 RNDr. Simon Toth (happy.cerberus@gmail.com) */

#include "../src/BlockChecker.h"
#include "../src/Square.h"
#include <catch2/catch.hpp>

namespace sudoku {

TEST_CASE("BlockChecker : Check", "") {
  std::vector<Square> actual{Square(3u), Square(3u), Square(3u)};
  BlockChecker blockChecker(
      std::vector<Square *>{&actual[0], &actual[1], &actual[2]});
  actual[0] = 1;
  actual[1] = 2;
  actual[2] = 3;
  REQUIRE(!blockChecker.HasConflict());
  actual[0].Reset();
  REQUIRE(!blockChecker.HasConflict());
  actual[0] = 3;
  REQUIRE(blockChecker.HasConflict());
}

TEST_CASE("BlockChecker : Prune", "") {
  std::vector<Square> actual{Square(3u), Square(3u), Square(3u)};
  BlockChecker blockChecker(
      std::vector<Square *>{&actual[0], &actual[1], &actual[2]});
  blockChecker.Prune(1u);
  blockChecker.Prune(2u);
  for (unsigned i = 0; i < 3u; i++) {
    REQUIRE(actual[i].Value() == 3u);
  }
  actual[0].Reset();
  blockChecker.Prune(1u);
  REQUIRE(actual[0].CountPossible() == 2u);
}

using Catch::Matchers::UnorderedEquals;

void TestOverrideValue(Square &s, uint64_t value) { s.OverrideValue(value); }

TEST_CASE("BlockChecker : recursive_set_find", "") {
  std::vector<Square> data(9, Square(9u));
  std::vector<Square *> squares;
  for (unsigned i = 0; i < static_cast<unsigned>(data.size()); i++) {
    squares.push_back(&data[i]);
  }

  {
    for (unsigned i = 0; i < static_cast<unsigned>(data.size()); i++) {
      data[i] = i + 1u;
    }
    std::vector<std::vector<unsigned>> result;
    recursive_set_find(result, squares, 1u);
    REQUIRE(result.size() == 9u);
    REQUIRE_THAT(result,
                 UnorderedEquals(std::vector<std::vector<unsigned>>{
                     std::vector<unsigned>{0u}, std::vector<unsigned>{1u},
                     std::vector<unsigned>{2u}, std::vector<unsigned>{3u},
                     std::vector<unsigned>{4u}, std::vector<unsigned>{5u},
                     std::vector<unsigned>{6u}, std::vector<unsigned>{7u},
                     std::vector<unsigned>{8u}}));
  }
  {
    TestOverrideValue(data[0], 0b11u);
    TestOverrideValue(data[1], 0b11u);
    TestOverrideValue(data[2], 0b100u);
    TestOverrideValue(data[3], 0b11000u);
    TestOverrideValue(data[4], 0b11000u);
    TestOverrideValue(data[5], 0b1100000u);
    TestOverrideValue(data[6], 0b110000000u);
    TestOverrideValue(data[7], 0b1100000u);
    TestOverrideValue(data[8], 0b110000000u);

    std::vector<std::vector<unsigned>> result;
    recursive_set_find(result, squares, 2u);
    REQUIRE(result.size() == 4lu);
    REQUIRE_THAT(
        result,
        UnorderedEquals(std::vector<std::vector<unsigned>>{
            std::vector<unsigned>{0u, 1u}, std::vector<unsigned>{3u, 4u},
            std::vector<unsigned>{5u, 7u}, std::vector<unsigned>{6u, 8u}}));
  }
}

TEST_CASE("BlockChecker : recursive_number_find", "") {
  std::vector<Square> data(9, Square(9u));
  std::vector<Square *> squares;
  for (unsigned i = 0; i < static_cast<unsigned>(data.size()); i++) {
    squares.push_back(&data[i]);
  }

  {
    for (unsigned i = 0; i < static_cast<unsigned>(data.size()); i++) {
      data[i] = i + 1u;
    }
    std::vector<std::vector<unsigned>> result;
    recursive_number_find(result, squares, 1);
    REQUIRE(result.size() == 9u);
    REQUIRE_THAT(result,
                 UnorderedEquals(std::vector<std::vector<unsigned>>{
                     std::vector<unsigned>{0u}, std::vector<unsigned>{1u},
                     std::vector<unsigned>{2u}, std::vector<unsigned>{3u},
                     std::vector<unsigned>{4u}, std::vector<unsigned>{5u},
                     std::vector<unsigned>{6u}, std::vector<unsigned>{7u},
                     std::vector<unsigned>{8u}}));
  }
  {
    TestOverrideValue(data[0], 0b111u);
    TestOverrideValue(data[1], 0b111u);
    TestOverrideValue(data[2], 0b100u);
    TestOverrideValue(data[3], 0b11000u);
    TestOverrideValue(data[4], 0b11000u);
    TestOverrideValue(data[5], 0b1100000u);
    TestOverrideValue(data[6], 0b111100000u);
    TestOverrideValue(data[7], 0b1100000u);
    TestOverrideValue(data[8], 0b111100000u);

    std::vector<std::vector<unsigned>> result;
    recursive_number_find(result, squares, 2);
    REQUIRE(result.size() == 3u);
    REQUIRE_THAT(result, UnorderedEquals(std::vector<std::vector<unsigned>>{
                             std::vector<unsigned>{0u, 1u},
                             std::vector<unsigned>{3u, 4u},
                             std::vector<unsigned>{7u, 8u}}));
  }
}
} // namespace sudoku