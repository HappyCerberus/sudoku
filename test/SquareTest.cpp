// (c) 2020 RNDr. Simon Toth (happy.cerberus@gmail.com)

#include "../src/Square.h"
#include <catch2/catch.hpp>

TEST_CASE("Square test : testing empty square", "") {
  sudoku::Square s(9u);
  REQUIRE(s.CountPossible() == 9u);
  REQUIRE(s.Value() == 0u);
  for (unsigned i = 1u; i <= 9u; i++) {
    REQUIRE(s.IsPossible(i));
  }
  REQUIRE(!s.IsSet());
  for (unsigned i = 0u; i < 9u; i++) {
    REQUIRE(s.Next(i) == i + 1u);
  }

  s = 3u;
  REQUIRE(s.Value() == 3u);
  REQUIRE(s.IsPossible(3u));
  REQUIRE(s.CountPossible() == 1u);
  REQUIRE(s.IsSet());
  REQUIRE(s.Next(0u) == 3u);
  REQUIRE(s.Next(3u) == 0u);

  s.Reset();
  REQUIRE(s.CountPossible() == 9u);
  REQUIRE(s.Value() == 0u);

  for (unsigned i = 1u; i <= 9u; i++) {
    REQUIRE(s.IsPossible(i)); // << "All values should be possible :" << i;
  }
  REQUIRE(!s.IsSet());
  for (unsigned i = 0u; i < 9u; i++) {
    REQUIRE(s.Next(i) == i + 1u);
  }
}
