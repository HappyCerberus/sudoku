/* (c) 2020 RNDr. Simon Toth (happy.cerberus@gmail.com) */

#include "BlockChecker.h"
#include "gtest/gtest.h"

namespace {
    TEST(BlockCheckerTest, Check
    ) {
        std::vector<SquareType> actual{SquareType(3), SquareType(3), SquareType(3)};
        BlockChecker blockChecker(std::vector<SquareType *>{&actual[0], &actual[1], &actual[2]});
        actual[0].Set(1);
        actual[1].Set(2);
        actual[2].Set(3);
        EXPECT_TRUE(blockChecker.Check());
        actual[0].Reset();
        EXPECT_TRUE(blockChecker.Check());
        actual[0].Set(3);
        EXPECT_FALSE(blockChecker.Check());
    }

    TEST(BlockCheckerTest, Prune
    ) {
        std::vector<SquareType> actual{SquareType(3), SquareType(3), SquareType(3)};
        BlockChecker blockChecker(std::vector<SquareType *>{&actual[0], &actual[1], &actual[2]});
        blockChecker.Prune(1);
        blockChecker.Prune(2);
        for (size_t i = 0; i < 3; i++) {
            EXPECT_EQ(actual[i].Value(), 3);
        }
        actual[0].Reset();
        blockChecker.Prune(1);
        EXPECT_EQ(actual[0].CountPossible(), 2);
    }
}