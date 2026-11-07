// Library modules Motor Interface (2.2) and Cleaner Interface (2.3): C12 compare-and-send.
// Test plan: INIT-03, INIT-04, C12-01 .. C12-07.
#include <gtest/gtest.h>
#include <vector>

extern "C" {
#include "motor_interface.h"
#include "cleaner_interface.h"
#include "fake_port.h"
}

namespace {

std::vector<int> Writes(PortWriteKind kind)
{
    std::vector<int> v;
    for (size_t i = 0; i < fake_port_write_count(); ++i) {
        PortWrite w = fake_port_write_at(i);
        if (w.kind == kind) {
            v.push_back(w.value);
        }
    }
    return v;
}

class Interfaces : public ::testing::Test {
protected:
    void SetUp() override
    {
        fake_port_reset();
        motor_interface_init();
        cleaner_interface_init();
    }
};

// C12-01 (C22, C27)
TEST_F(Interfaces, C12_01_MotorFirstSendIsWritten)
{
    motor_interface_send(MOTOR_FORWARD);
    EXPECT_EQ(Writes(PORT_WRITE_DIRECTION), (std::vector<int>{MOTOR_FORWARD}));
    EXPECT_TRUE(Writes(PORT_WRITE_CLEAN).empty());
}

// C12-02 (C12)
TEST_F(Interfaces, C12_02_MotorSameValueNotResent)
{
    motor_interface_send(MOTOR_TURN_LEFT);
    motor_interface_send(MOTOR_TURN_LEFT);
    EXPECT_EQ(fake_port_write_count(), 1u);
}

// C12-03 (C12)
TEST_F(Interfaces, C12_03_MotorOnlyChangesWritten)
{
    motor_interface_send(MOTOR_FORWARD);
    motor_interface_send(MOTOR_TURN_LEFT);
    motor_interface_send(MOTOR_TURN_LEFT);
    motor_interface_send(MOTOR_FORWARD);
    EXPECT_EQ(Writes(PORT_WRITE_DIRECTION),
              (std::vector<int>{MOTOR_FORWARD, MOTOR_TURN_LEFT, MOTOR_FORWARD}));
}

// C12-04 (DD 4.4)
TEST_F(Interfaces, C12_04_MotorAllFourValues)
{
    motor_interface_send(MOTOR_FORWARD);
    motor_interface_send(MOTOR_BACKWARD);
    motor_interface_send(MOTOR_TURN_LEFT);
    motor_interface_send(MOTOR_TURN_RIGHT);
    EXPECT_EQ(Writes(PORT_WRITE_DIRECTION),
              (std::vector<int>{MOTOR_FORWARD, MOTOR_BACKWARD, MOTOR_TURN_LEFT, MOTOR_TURN_RIGHT}));
}

// INIT-03 (C27, PS 2.2)
TEST_F(Interfaces, INIT03_MotorInitForgetsLastSent)
{
    motor_interface_send(MOTOR_FORWARD);
    motor_interface_init();
    motor_interface_send(MOTOR_FORWARD);
    EXPECT_EQ(Writes(PORT_WRITE_DIRECTION), (std::vector<int>{MOTOR_FORWARD, MOTOR_FORWARD}));
}

// C12-05 (C22, C27)
TEST_F(Interfaces, C12_05_CleanerFirstSendIsWritten)
{
    cleaner_interface_send(CLEAN_ON);
    EXPECT_EQ(Writes(PORT_WRITE_CLEAN), (std::vector<int>{CLEAN_ON}));
    EXPECT_TRUE(Writes(PORT_WRITE_DIRECTION).empty());
}

// C12-06 (C12)
TEST_F(Interfaces, C12_06_CleanerSameValueNotResent)
{
    cleaner_interface_send(CLEAN_OFF);
    cleaner_interface_send(CLEAN_OFF);
    EXPECT_EQ(fake_port_write_count(), 1u);
}

// C12-07 (C12)
TEST_F(Interfaces, C12_07_CleanerOnlyChangesWritten)
{
    cleaner_interface_send(CLEAN_ON);
    cleaner_interface_send(CLEAN_OFF);
    cleaner_interface_send(CLEAN_OFF);
    cleaner_interface_send(CLEAN_UP);
    cleaner_interface_send(CLEAN_ON);
    EXPECT_EQ(Writes(PORT_WRITE_CLEAN),
              (std::vector<int>{CLEAN_ON, CLEAN_OFF, CLEAN_UP, CLEAN_ON}));
}

// INIT-04 (C27, PS 2.3)
TEST_F(Interfaces, INIT04_CleanerInitForgetsLastSent)
{
    cleaner_interface_send(CLEAN_ON);
    cleaner_interface_init();
    cleaner_interface_send(CLEAN_ON);
    EXPECT_EQ(Writes(PORT_WRITE_CLEAN), (std::vector<int>{CLEAN_ON, CLEAN_ON}));
}

} // namespace
