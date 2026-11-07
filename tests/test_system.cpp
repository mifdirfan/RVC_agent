// RVC System through the same entry points as main(): rvc_startup() and rvc_tick().
// All production modules are linked; only the environment ports are doubles.
// Asserts the exact Direction / Clean write sequence and count (TDD rule 8).
// Unit test cases (artifacts/TDD/unit_test_cases.md): INIT-08, SYS-01 .. SYS-10.
#include <gtest/gtest.h>
#include <vector>
#include <utility>

extern "C" {
#include "rvc.h"
#include "front_sensor.h"
#include "obstacle_location.h"
#include "dust_existence.h"
#include "fake_port.h"
}

namespace {

using Write = std::pair<int, int>; // (PortWriteKind, value)
using Writes = std::vector<Write>;

const Write DIR_FWD{PORT_WRITE_DIRECTION, MOTOR_FORWARD};
const Write DIR_BACK{PORT_WRITE_DIRECTION, MOTOR_BACKWARD};
const Write DIR_TL{PORT_WRITE_DIRECTION, MOTOR_TURN_LEFT};
const Write DIR_TR{PORT_WRITE_DIRECTION, MOTOR_TURN_RIGHT};
const Write CLN_ON{PORT_WRITE_CLEAN, CLEAN_ON};
const Write CLN_OFF{PORT_WRITE_CLEAN, CLEAN_OFF};
const Write CLN_UP{PORT_WRITE_CLEAN, CLEAN_UP};

constexpr bool B = true;
constexpr bool o = false;

class System : public ::testing::Test {
protected:
    void SetUp() override
    {
        fake_port_reset();
        rvc_startup();
    }

    static Writes Log()
    {
        Writes w;
        for (size_t i = 0; i < fake_port_write_count(); ++i) {
            PortWrite p = fake_port_write_at(i);
            w.emplace_back(static_cast<int>(p.kind), p.value);
        }
        return w;
    }

    // One Tick with the periodic sensor values the port returns on that Tick.
    static Writes Tick(bool left, bool right, bool dust)
    {
        fake_port_clear_writes();
        fake_port_set_left(left);
        fake_port_set_right(right);
        fake_port_set_dust(dust);
        rvc_tick();
        return Log();
    }

    static Writes Ticks(int n, bool left, bool right, bool dust)
    {
        Writes all;
        for (int i = 0; i < n; ++i) {
            Writes w = Tick(left, right, dust);
            all.insert(all.end(), w.begin(), w.end());
        }
        return all;
    }
};

// SYS-01 (C12, C16, C22, C27)
TEST_F(System, SYS01_StartupSendsForwardAndCleanOnOnceThenNothing)
{
    EXPECT_EQ(Log(), (Writes{DIR_FWD, CLN_ON}));
    EXPECT_EQ(Ticks(10, o, o, o), Writes{});
}

// INIT-08 (C21, C22)
TEST_F(System, INIT08_StartupResetsStoresAndInterfaces)
{
    front_sensor_on_interrupt(true);
    Tick(B, B, B); // reversing, Clean off
    fake_port_reset();
    rvc_startup();
    EXPECT_EQ(Log(), (Writes{DIR_FWD, CLN_ON}));
    ObstacleLocation obs = obstacle_location_get();
    EXPECT_FALSE(obs.front);
    EXPECT_FALSE(obs.left);
    EXPECT_FALSE(obs.right);
    EXPECT_FALSE(dust_existence_get());
}

// SYS-02 (R3, C4, C5, C8, C20)
TEST_F(System, SYS02_FrontObstacleTurnLeftThreeTicksThenForward)
{
    front_sensor_on_interrupt(true);
    EXPECT_EQ(Tick(o, o, o), (Writes{CLN_OFF, DIR_TL}));
    EXPECT_EQ(Ticks(2, o, o, o), Writes{});
    front_sensor_on_interrupt(false);
    EXPECT_EQ(Tick(o, o, o), (Writes{DIR_FWD, CLN_ON}));
}

// SYS-03 (C1, C20)
TEST_F(System, SYS03_InterruptActedOnAtNextTickOnly)
{
    fake_port_clear_writes();
    front_sensor_on_interrupt(true);
    EXPECT_EQ(Log(), Writes{});
    EXPECT_EQ(Tick(o, o, o), (Writes{CLN_OFF, DIR_TL}));
}

// SYS-04 (C18, C20): left sampled on this Tick, before the decision
TEST_F(System, SYS04_SidesSampledOnSameTickBeforeDecision)
{
    front_sensor_on_interrupt(true);
    EXPECT_EQ(Tick(B, o, o), (Writes{CLN_OFF, DIR_TR}));
    EXPECT_EQ(fake_port_left_reads(), 1);
    EXPECT_EQ(fake_port_right_reads(), 1);
    EXPECT_EQ(fake_port_dust_reads(), 1);
}

// SYS-05 (R4, C9)
TEST_F(System, SYS05_AllBlockedReverseUntilSideOpensThenTurnThenForward)
{
    front_sensor_on_interrupt(true);
    EXPECT_EQ(Tick(B, B, o), (Writes{CLN_OFF, DIR_BACK}));
    EXPECT_EQ(Ticks(4, B, B, o), Writes{}); // >= 3 ticks, both sides still blocked
    EXPECT_EQ(Tick(o, B, o), (Writes{DIR_TL}));
    front_sensor_on_interrupt(false);
    EXPECT_EQ(Ticks(2, o, o, o), Writes{});
    EXPECT_EQ(Tick(o, o, o), (Writes{DIR_FWD, CLN_ON}));
}

// SYS-06 (C12, C15, T13)
TEST_F(System, SYS06_TurnReentryDoesNotResendTurnLeft)
{
    front_sensor_on_interrupt(true);
    EXPECT_EQ(Tick(o, o, o), (Writes{CLN_OFF, DIR_TL}));
    EXPECT_EQ(Ticks(3, o, o, o), Writes{}); // T12, T12, T13 (Turn Left filtered by C12)
    front_sensor_on_interrupt(false);
    EXPECT_EQ(Ticks(2, o, o, o), Writes{}); // new turn still running
    EXPECT_EQ(Tick(o, o, o), (Writes{DIR_FWD, CLN_ON}));
}

// SYS-07 (R5, C6)
TEST_F(System, SYS07_DustPowersUpForThreeTicks)
{
    EXPECT_EQ(Tick(o, o, B), (Writes{CLN_UP}));
    EXPECT_EQ(Ticks(2, o, o, B), Writes{});
    EXPECT_EQ(Tick(o, o, o), (Writes{CLN_ON}));
}

// SYS-08 (C11)
TEST_F(System, SYS08_ObstacleDuringPowerUpCancelsIt)
{
    EXPECT_EQ(Tick(o, o, B), (Writes{CLN_UP}));
    front_sensor_on_interrupt(true);
    EXPECT_EQ(Tick(o, o, B), (Writes{CLN_OFF, DIR_TL}));
}

// SYS-09 (C19)
TEST_F(System, SYS09_FrontFalseInterruptCausesNoAction)
{
    front_sensor_on_interrupt(false);
    EXPECT_EQ(Ticks(3, o, o, o), Writes{});
}

// SYS-10 (C4, T19)
TEST_F(System, SYS10_TurnRightThenTurnLeftWhenLeftOpens)
{
    front_sensor_on_interrupt(true);
    EXPECT_EQ(Tick(B, o, o), (Writes{CLN_OFF, DIR_TR}));
    EXPECT_EQ(Ticks(2, B, o, o), Writes{});
    EXPECT_EQ(Tick(o, o, o), (Writes{DIR_TL}));
}

} // namespace
