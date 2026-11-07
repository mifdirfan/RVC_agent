// Controller (2.1.1) = Level 4 FSM, T1 .. T25 (dfd_level_4_fsm.md, SC Section 7).
// The Controller is linked with fake stores and fake action modules, so each test
// asserts the exact Enable / Disable / Trigger sequence of a transition.
// Test plan: INIT-07, T1 .. T25, CL-C10/C11/C13/C14/C28/C29, CNT-01 .. CNT-05.
#include <gtest/gtest.h>
#include <vector>

extern "C" {
#include "controller.h"
#include "fake_stores.h"
#include "fake_actions.h"
}

namespace {

using Sigs = std::vector<Signal>;

constexpr bool B = true;  // blocked / dust
constexpr bool o = false; // free / no dust

class Controller : public ::testing::Test {
protected:
    void SetUp() override
    {
        fake_stores_reset();
        fake_actions_reset();
        controller_init(); // T1: FwdNormal
    }

    static Sigs Signals()
    {
        Sigs s;
        for (size_t i = 0; i < fake_actions_count(); ++i) {
            s.push_back(fake_actions_at(i));
        }
        return s;
    }

    // One Tick with the store values the Controller reads on that Tick.
    static Sigs Tick(bool f, bool l, bool r, bool d)
    {
        fake_actions_reset();
        fake_stores_set(f, l, r, d);
        controller_tick();
        return Signals();
    }

    // n Ticks with the same store values; returns all signals issued.
    static Sigs Ticks(int n, bool f, bool l, bool r, bool d)
    {
        Sigs all;
        for (int i = 0; i < n; ++i) {
            Sigs s = Tick(f, l, r, d);
            all.insert(all.end(), s.begin(), s.end());
        }
        return all;
    }

    // Entry into each state from FwdNormal (state on return: the named state).
    static void ReachPowerUp() { ASSERT_EQ(Tick(o, o, o, B), (Sigs{SIG_PU})); }
    static void ReachTurnLeft() { ASSERT_EQ(Tick(B, o, o, o), (Sigs{SIG_MF_DISABLE, SIG_CO, SIG_TL})); }
    static void ReachTurnRight() { ASSERT_EQ(Tick(B, B, o, o), (Sigs{SIG_MF_DISABLE, SIG_CO, SIG_TR})); }
    static void ReachBackward() { ASSERT_EQ(Tick(B, B, B, o), (Sigs{SIG_MF_DISABLE, SIG_CO, SIG_MB_ENABLE})); }

    // FwdNormal check: dust with front clear gives T6 only from FwdNormal.
    static void ExpectFwdNormal() { EXPECT_EQ(Tick(o, o, o, B), (Sigs{SIG_PU})); }
    // TurnLeft check (just entered): 2 silent ticks, then front clear exits with T16.
    static void ExpectTurnJustEntered()
    {
        EXPECT_EQ(Ticks(2, o, o, o, o), Sigs{});
        EXPECT_EQ(Tick(o, o, o, o), (Sigs{SIG_MF_ENABLE, SIG_CN}));
    }
};

// ---------------------------------------------------------------- T1 / INIT-07
TEST_F(Controller, T1_InitEnablesMoveForwardThenTriggersCleanNormal)
{
    fake_actions_reset();
    controller_init();
    EXPECT_EQ(Signals(), (Sigs{SIG_MF_ENABLE, SIG_CN}));
    ExpectFwdNormal();
}

TEST_F(Controller, INIT07_InitResetsStateToFwdNormal)
{
    ReachBackward();
    fake_actions_reset();
    controller_init();
    EXPECT_EQ(Signals(), (Sigs{SIG_MF_ENABLE, SIG_CN}));
    ExpectFwdNormal();
}

// ---------------------------------------------------------------- FwdNormal
TEST_F(Controller, T2_FwdNormalFrontBlockedLeftFreeTurnsLeft)
{
    EXPECT_EQ(Tick(B, o, B, B), (Sigs{SIG_MF_DISABLE, SIG_CO, SIG_TL}));
    ExpectTurnJustEntered();
}

TEST_F(Controller, T2_LeftFirstWhenBothSidesFree)
{
    EXPECT_EQ(Tick(B, o, o, o), (Sigs{SIG_MF_DISABLE, SIG_CO, SIG_TL}));
}

TEST_F(Controller, T3_FwdNormalFrontAndLeftBlockedTurnsRight)
{
    EXPECT_EQ(Tick(B, B, o, B), (Sigs{SIG_MF_DISABLE, SIG_CO, SIG_TR}));
    ExpectTurnJustEntered();
}

TEST_F(Controller, T4_FwdNormalAllBlockedMovesBackward)
{
    EXPECT_EQ(Tick(B, B, B, B), (Sigs{SIG_MF_DISABLE, SIG_CO, SIG_MB_ENABLE}));
    // MoveBackward: 2 silent ticks even with a side open, 3rd tick T22.
    EXPECT_EQ(Ticks(2, o, o, o, o), Sigs{});
    EXPECT_EQ(Tick(o, o, o, o), (Sigs{SIG_MB_DISABLE, SIG_TL}));
}

TEST_F(Controller, T5_FwdNormalFrontClearNoDustNoSignalForAnySide)
{
    EXPECT_EQ(Tick(o, o, o, o), Sigs{});
    EXPECT_EQ(Tick(o, B, o, o), Sigs{});
    EXPECT_EQ(Tick(o, o, B, o), Sigs{});
    EXPECT_EQ(Tick(o, B, B, o), Sigs{});
    EXPECT_EQ(fake_stores_obstacle_gets(), 4); // stores read on every Tick (C18, C20)
    EXPECT_EQ(fake_stores_dust_gets(), 4);
    ExpectFwdNormal();
}

TEST_F(Controller, T6_FwdNormalDustStartsPowerUp)
{
    EXPECT_EQ(Tick(o, B, B, B), (Sigs{SIG_PU}));
    // In FwdPowerUp: 2 silent ticks, 3rd tick T11.
    EXPECT_EQ(Ticks(2, o, o, o, o), Sigs{});
    EXPECT_EQ(Tick(o, o, o, o), (Sigs{SIG_CN}));
}

// ---------------------------------------------------------------- FwdPowerUp
TEST_F(Controller, T7_PowerUpFrontBlockedLeftFreeCancelsAndTurnsLeft)
{
    ReachPowerUp();
    EXPECT_EQ(Tick(B, o, o, B), (Sigs{SIG_MF_DISABLE, SIG_CO, SIG_TL}));
    ExpectTurnJustEntered();
}

TEST_F(Controller, T8_PowerUpFrontAndLeftBlockedCancelsAndTurnsRight)
{
    ReachPowerUp();
    EXPECT_EQ(Tick(B, B, o, B), (Sigs{SIG_MF_DISABLE, SIG_CO, SIG_TR}));
    ExpectTurnJustEntered();
}

TEST_F(Controller, T9_PowerUpAllBlockedCancelsAndMovesBackward)
{
    ReachPowerUp();
    EXPECT_EQ(Tick(B, B, B, B), (Sigs{SIG_MF_DISABLE, SIG_CO, SIG_MB_ENABLE}));
    EXPECT_EQ(Ticks(2, o, o, o, o), Sigs{});
    EXPECT_EQ(Tick(o, B, o, o), (Sigs{SIG_MB_DISABLE, SIG_TR}));
}

TEST_F(Controller, T10_PowerUpStaysForTwoTicksIgnoringDustAndSides)
{
    ReachPowerUp();
    EXPECT_EQ(Tick(o, B, o, B), Sigs{});
    EXPECT_EQ(Tick(o, o, B, o), Sigs{});
}

TEST_F(Controller, T11_PowerUpEndsOnThirdTick)
{
    ReachPowerUp();
    EXPECT_EQ(Ticks(2, o, o, o, o), Sigs{});
    EXPECT_EQ(Tick(o, o, o, B), (Sigs{SIG_CN}));
}

// ---------------------------------------------------------------- TurnLeft
TEST_F(Controller, T12_TurnLeftIgnoresFrontAndDustForTwoTicks)
{
    ReachTurnLeft();
    EXPECT_EQ(Tick(B, B, B, B), Sigs{});
    EXPECT_EQ(Tick(B, o, o, B), Sigs{});
}

TEST_F(Controller, T13_TurnLeftEndsFrontBlockedLeftFreeReentersTurnLeft)
{
    ReachTurnLeft();
    EXPECT_EQ(Ticks(2, o, o, o, o), Sigs{});
    EXPECT_EQ(Tick(B, o, B, o), (Sigs{SIG_TL}));
    ExpectTurnJustEntered(); // CNT-05: counter reset on re-entry
}

TEST_F(Controller, T14_TurnLeftEndsFrontAndLeftBlockedTurnsRight)
{
    ReachTurnLeft();
    EXPECT_EQ(Ticks(2, o, o, o, o), Sigs{});
    EXPECT_EQ(Tick(B, B, o, o), (Sigs{SIG_TR}));
    ExpectTurnJustEntered();
}

TEST_F(Controller, T15_TurnLeftEndsAllBlockedMovesBackward)
{
    ReachTurnLeft();
    EXPECT_EQ(Ticks(2, o, o, o, o), Sigs{});
    EXPECT_EQ(Tick(B, B, B, o), (Sigs{SIG_MB_ENABLE}));
    EXPECT_EQ(Ticks(2, o, o, o, o), Sigs{});
    EXPECT_EQ(Tick(o, o, o, o), (Sigs{SIG_MB_DISABLE, SIG_TL}));
}

TEST_F(Controller, T16_TurnLeftEndsFrontClearReturnsForwardIgnoringDust)
{
    ReachTurnLeft();
    EXPECT_EQ(Ticks(2, o, o, o, B), Sigs{});
    EXPECT_EQ(Tick(o, o, o, B), (Sigs{SIG_MF_ENABLE, SIG_CN})); // C29: no PU
    ExpectFwdNormal();
}

// ---------------------------------------------------------------- TurnRight
TEST_F(Controller, T17_TurnRightIgnoresFrontAndDustForTwoTicks)
{
    ReachTurnRight();
    EXPECT_EQ(Tick(B, B, B, B), Sigs{});
    EXPECT_EQ(Tick(B, o, o, B), Sigs{});
}

TEST_F(Controller, T18_TurnRightEndsFrontAndLeftBlockedReentersTurnRight)
{
    ReachTurnRight();
    EXPECT_EQ(Ticks(2, o, o, o, o), Sigs{});
    EXPECT_EQ(Tick(B, B, o, o), (Sigs{SIG_TR}));
    ExpectTurnJustEntered(); // counter reset on re-entry
}

TEST_F(Controller, T19_TurnRightEndsFrontBlockedLeftFreeTurnsLeft)
{
    ReachTurnRight();
    EXPECT_EQ(Ticks(2, o, o, o, o), Sigs{});
    EXPECT_EQ(Tick(B, o, o, o), (Sigs{SIG_TL}));
    ExpectTurnJustEntered();
}

TEST_F(Controller, T20_TurnRightEndsAllBlockedMovesBackward)
{
    ReachTurnRight();
    EXPECT_EQ(Ticks(2, o, o, o, o), Sigs{});
    EXPECT_EQ(Tick(B, B, B, o), (Sigs{SIG_MB_ENABLE}));
    EXPECT_EQ(Ticks(2, o, o, o, o), Sigs{});
    EXPECT_EQ(Tick(o, o, o, o), (Sigs{SIG_MB_DISABLE, SIG_TL}));
}

TEST_F(Controller, T21_TurnRightEndsFrontClearReturnsForwardIgnoringDust)
{
    ReachTurnRight();
    EXPECT_EQ(Ticks(2, o, o, o, B), Sigs{});
    EXPECT_EQ(Tick(o, B, B, B), (Sigs{SIG_MF_ENABLE, SIG_CN})); // C29: no PU
    ExpectFwdNormal();
}

// ---------------------------------------------------------------- MoveBackward
TEST_F(Controller, T22_ReverseEndsLeftFreeTurnsLeft)
{
    ReachBackward();
    EXPECT_EQ(Ticks(2, B, B, B, o), Sigs{});
    EXPECT_EQ(Tick(B, o, B, B), (Sigs{SIG_MB_DISABLE, SIG_TL})); // F, D not tested
    ExpectTurnJustEntered();
}

TEST_F(Controller, T23_ReverseEndsLeftBlockedRightFreeTurnsRight)
{
    ReachBackward();
    EXPECT_EQ(Ticks(2, B, B, B, o), Sigs{});
    EXPECT_EQ(Tick(B, B, o, B), (Sigs{SIG_MB_DISABLE, SIG_TR}));
    ExpectTurnJustEntered();
}

TEST_F(Controller, T24_ReverseLastsAtLeastThreeTicksIgnoringInputs)
{
    ReachBackward();
    EXPECT_EQ(Tick(o, o, o, B), Sigs{});
    EXPECT_EQ(Tick(B, o, o, B), Sigs{});
}

TEST_F(Controller, T25_ReverseContinuesWhileBothSidesBlocked)
{
    ReachBackward();
    EXPECT_EQ(Ticks(2, B, B, B, o), Sigs{});
    EXPECT_EQ(Ticks(5, o, B, B, B), Sigs{});
    EXPECT_EQ(Tick(o, B, o, o), (Sigs{SIG_MB_DISABLE, SIG_TR}));
}

// ---------------------------------------------------------------- clarifications
TEST_F(Controller, CL_C13_SideOnlyObstacleNoReactionInForwardStates)
{
    EXPECT_EQ(Ticks(3, o, B, B, o), Sigs{});
    ReachPowerUp();
    EXPECT_EQ(Ticks(2, o, B, o, o), Sigs{});
}

TEST_F(Controller, CL_C14_DustDuringPowerUpDoesNotRestart)
{
    ReachPowerUp();
    EXPECT_EQ(Ticks(2, o, o, o, B), Sigs{});
    EXPECT_EQ(Tick(o, o, o, B), (Sigs{SIG_CN})); // ends after 3 ticks, not extended
}

TEST_F(Controller, CL_C28_DustAfterPowerUpEndsStartsNewPowerUpNextTick)
{
    ReachPowerUp();
    EXPECT_EQ(Ticks(2, o, o, o, B), Sigs{});
    EXPECT_EQ(Tick(o, o, o, B), (Sigs{SIG_CN}));
    EXPECT_EQ(Tick(o, o, o, B), (Sigs{SIG_PU}));
}

TEST_F(Controller, CL_C29_DustOnTurnEndTickActedOnNextTick)
{
    ReachTurnLeft();
    EXPECT_EQ(Ticks(2, B, o, o, B), Sigs{});
    EXPECT_EQ(Tick(o, o, o, B), (Sigs{SIG_MF_ENABLE, SIG_CN}));
    EXPECT_EQ(Tick(o, o, o, B), (Sigs{SIG_PU}));
}

TEST_F(Controller, CL_C10_DustIgnoredWhileReversingAndTurning)
{
    ReachBackward();
    EXPECT_EQ(Ticks(4, o, B, B, B), Sigs{});
    EXPECT_EQ(Tick(o, o, B, B), (Sigs{SIG_MB_DISABLE, SIG_TL}));
    EXPECT_EQ(Ticks(2, o, o, o, B), Sigs{});
    EXPECT_EQ(Tick(o, o, o, B), (Sigs{SIG_MF_ENABLE, SIG_CN}));
}

TEST_F(Controller, CL_C11_ObstacleCancelsPowerUpAndNextPowerUpIsFull)
{
    ReachPowerUp();
    EXPECT_EQ(Tick(o, o, o, o), Sigs{}); // power-up tick 1
    EXPECT_EQ(Tick(B, o, o, o), (Sigs{SIG_MF_DISABLE, SIG_CO, SIG_TL})); // T7
    EXPECT_EQ(Ticks(2, o, o, o, o), Sigs{});
    EXPECT_EQ(Tick(o, o, o, o), (Sigs{SIG_MF_ENABLE, SIG_CN})); // T16
    ReachPowerUp(); // T6: counter reset
    EXPECT_EQ(Ticks(2, o, o, o, o), Sigs{});
    EXPECT_EQ(Tick(o, o, o, o), (Sigs{SIG_CN}));
}

// ---------------------------------------------------------------- counters
TEST_F(Controller, CNT01_TurnLastsExactlyThreeTicks)
{
    ReachTurnRight();
    EXPECT_EQ(Tick(o, o, o, o), Sigs{});
    EXPECT_EQ(Tick(o, o, o, o), Sigs{});
    EXPECT_EQ(Tick(o, o, o, o), (Sigs{SIG_MF_ENABLE, SIG_CN}));
}

TEST_F(Controller, CNT02_PowerUpLastsExactlyThreeTicks)
{
    ReachPowerUp();
    EXPECT_EQ(Tick(o, o, o, o), Sigs{});
    EXPECT_EQ(Tick(o, o, o, o), Sigs{});
    EXPECT_EQ(Tick(o, o, o, o), (Sigs{SIG_CN}));
}

TEST_F(Controller, CNT03_ReverseMinimumThreeTicksEvenIfSideOpensAtOnce)
{
    ReachBackward();
    EXPECT_EQ(Tick(o, o, o, o), Sigs{});
    EXPECT_EQ(Tick(o, o, o, o), Sigs{});
    EXPECT_EQ(Tick(o, o, o, o), (Sigs{SIG_MB_DISABLE, SIG_TL}));
}

TEST_F(Controller, CNT04_ReverseCounterSaturatesBeyondUint8Range)
{
    ReachBackward();
    EXPECT_EQ(Ticks(300, B, B, B, B), Sigs{});
    EXPECT_EQ(Tick(B, o, o, o), (Sigs{SIG_MB_DISABLE, SIG_TL}));
}

TEST_F(Controller, CNT05_TurnCounterResetOnReentryRepeatedly)
{
    ReachTurnLeft();
    for (int i = 0; i < 3; ++i) {
        EXPECT_EQ(Ticks(2, B, o, o, o), Sigs{});
        EXPECT_EQ(Tick(B, o, o, o), (Sigs{SIG_TL}));
    }
    ExpectTurnJustEntered();
}

} // namespace
