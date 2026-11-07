// Decide Avoidance: left first (C4), backward when both sides blocked (R4).
// Unit test cases (artifacts/TDD/unit_test_cases.md): AV-01 .. AV-04.
#include <gtest/gtest.h>

extern "C" {
#include "avoidance.h"
}

TEST(Avoidance, AV01_BothFreeTurnsLeft) { EXPECT_EQ(decide_avoidance(false, false), AVOID_TURN_LEFT); }
TEST(Avoidance, AV02_LeftFreeRightBlockedTurnsLeft) { EXPECT_EQ(decide_avoidance(false, true), AVOID_TURN_LEFT); }
TEST(Avoidance, AV03_LeftBlockedRightFreeTurnsRight) { EXPECT_EQ(decide_avoidance(true, false), AVOID_TURN_RIGHT); }
TEST(Avoidance, AV04_BothBlockedBackward) { EXPECT_EQ(decide_avoidance(true, true), AVOID_BACKWARD); }
