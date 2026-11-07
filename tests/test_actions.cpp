// Efferent action modules 2.1.2 .. 2.1.8 (C24, C25).
// Test plan: INIT-05, INIT-06, ACT-01 .. ACT-09.
#include <gtest/gtest.h>

extern "C" {
#include "move_forward.h"
#include "move_backward.h"
#include "turn_left.h"
#include "turn_right.h"
#include "clean_normal.h"
#include "clean_off.h"
#include "power_up_clean.h"
#include "fake_interfaces.h"
}

namespace {

class Actions : public ::testing::Test {
protected:
    void SetUp() override
    {
        fake_interfaces_reset();
        move_forward_init();
        move_backward_init();
    }
};

void ExpectCall(size_t i, InterfaceKind kind, int value)
{
    ASSERT_LT(i, fake_interfaces_count());
    InterfaceCall c = fake_interfaces_at(i);
    EXPECT_EQ(c.kind, kind);
    EXPECT_EQ(c.value, value);
}

// INIT-05, INIT-06 (PS 2.1.2, 2.1.5)
TEST_F(Actions, INIT05_06_InitSendsNothing)
{
    move_forward_init();
    move_backward_init();
    EXPECT_EQ(fake_interfaces_count(), 0u);
}

// ACT-01 (C24)
TEST_F(Actions, ACT01_MoveForwardEnableSendsForwardOnce)
{
    move_forward_enable();
    ASSERT_EQ(fake_interfaces_count(), 1u);
    ExpectCall(0, IF_MOTOR, MOTOR_FORWARD);
}

// ACT-02 (C24)
TEST_F(Actions, ACT02_MoveForwardDisableSendsNothing)
{
    move_forward_enable();
    move_forward_disable();
    EXPECT_EQ(fake_interfaces_count(), 1u);
}

// ACT-03 (C24)
TEST_F(Actions, ACT03_MoveBackwardEnableSendsBackwardOnce)
{
    move_backward_enable();
    ASSERT_EQ(fake_interfaces_count(), 1u);
    ExpectCall(0, IF_MOTOR, MOTOR_BACKWARD);
}

// ACT-04 (C24)
TEST_F(Actions, ACT04_MoveBackwardDisableSendsNothing)
{
    move_backward_enable();
    move_backward_disable();
    EXPECT_EQ(fake_interfaces_count(), 1u);
}

// ACT-05 (C25): one command per Trigger
TEST_F(Actions, ACT05_TurnLeftOnePerTrigger)
{
    turn_left_trigger();
    turn_left_trigger();
    ASSERT_EQ(fake_interfaces_count(), 2u);
    ExpectCall(0, IF_MOTOR, MOTOR_TURN_LEFT);
    ExpectCall(1, IF_MOTOR, MOTOR_TURN_LEFT);
}

// ACT-06 (C25)
TEST_F(Actions, ACT06_TurnRightOnePerTrigger)
{
    turn_right_trigger();
    ASSERT_EQ(fake_interfaces_count(), 1u);
    ExpectCall(0, IF_MOTOR, MOTOR_TURN_RIGHT);
}

// ACT-07 (C25)
TEST_F(Actions, ACT07_CleanNormalSendsCleanOn)
{
    clean_normal_trigger();
    ASSERT_EQ(fake_interfaces_count(), 1u);
    ExpectCall(0, IF_CLEANER, CLEAN_ON);
}

// ACT-08 (C25)
TEST_F(Actions, ACT08_CleanOffSendsCleanOff)
{
    clean_off_trigger();
    ASSERT_EQ(fake_interfaces_count(), 1u);
    ExpectCall(0, IF_CLEANER, CLEAN_OFF);
}

// ACT-09 (C25)
TEST_F(Actions, ACT09_PowerUpCleanSendsCleanUp)
{
    power_up_clean_trigger();
    ASSERT_EQ(fake_interfaces_count(), 1u);
    ExpectCall(0, IF_CLEANER, CLEAN_UP);
}

} // namespace
