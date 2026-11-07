// Afferent branch (Process 1): sensor interfaces, store modules, Detect Obstacle & Dust.
// Unit test cases (artifacts/TDD/unit_test_cases.md): INIT-01, INIT-02, AFF-01 .. AFF-09.
#include <gtest/gtest.h>

extern "C" {
#include "left_sensor.h"
#include "right_sensor.h"
#include "dust_sensor.h"
#include "front_sensor.h"
#include "obstacle_location.h"
#include "dust_existence.h"
#include "detection.h"
#include "fake_port.h"
}

namespace {

class Afferent : public ::testing::Test {
protected:
    void SetUp() override
    {
        fake_port_reset();
        obstacle_location_init();
        dust_existence_init();
    }
};

void ExpectObstacle(bool f, bool l, bool r)
{
    ObstacleLocation o = obstacle_location_get();
    EXPECT_EQ(o.front, f);
    EXPECT_EQ(o.left, l);
    EXPECT_EQ(o.right, r);
}

// INIT-01 (C21)
TEST_F(Afferent, INIT01_ObstacleLocationInitIsAllFalse)
{
    obstacle_location_set_front(true);
    obstacle_location_set_sides(true, true);
    obstacle_location_init();
    ExpectObstacle(false, false, false);
}

// INIT-02 (C21)
TEST_F(Afferent, INIT02_DustExistenceInitIsFalse)
{
    dust_existence_set(true);
    dust_existence_init();
    EXPECT_FALSE(dust_existence_get());
}

// AFF-01 (PS 1.2, C3)
TEST_F(Afferent, AFF01_LeftSensorReturnsPortValueOncePerCall)
{
    fake_port_set_left(true);
    EXPECT_TRUE(left_sensor_sample());
    fake_port_set_left(false);
    EXPECT_FALSE(left_sensor_sample());
    EXPECT_EQ(fake_port_left_reads(), 2);
}

// AFF-02 (PS 1.3, C3)
TEST_F(Afferent, AFF02_RightSensorReturnsPortValueOncePerCall)
{
    fake_port_set_right(true);
    EXPECT_TRUE(right_sensor_sample());
    fake_port_set_right(false);
    EXPECT_FALSE(right_sensor_sample());
    EXPECT_EQ(fake_port_right_reads(), 2);
}

// AFF-03 (PS 1.4, C3)
TEST_F(Afferent, AFF03_DustSensorReturnsPortValueOncePerCall)
{
    fake_port_set_dust(true);
    EXPECT_TRUE(dust_sensor_sample());
    fake_port_set_dust(false);
    EXPECT_FALSE(dust_sensor_sample());
    EXPECT_EQ(fake_port_dust_reads(), 2);
}

// AFF-04 (PS 1.1, 1.5, C1)
TEST_F(Afferent, AFF04_FrontInterruptTrueSetsOnlyFront)
{
    obstacle_location_set_sides(true, false);
    front_sensor_on_interrupt(true);
    ExpectObstacle(true, true, false);
}

// AFF-05 (C19)
TEST_F(Afferent, AFF05_FrontInterruptFalseIsPassedOn)
{
    front_sensor_on_interrupt(true);
    front_sensor_on_interrupt(false);
    ExpectObstacle(false, false, false);
}

// AFF-06 (PS 1.5)
TEST_F(Afferent, AFF06_SetSidesKeepsFront)
{
    obstacle_location_set_front(true);
    obstacle_location_set_sides(true, false);
    ExpectObstacle(true, true, false);
    obstacle_location_set_sides(false, true);
    ExpectObstacle(true, false, true);
}

// AFF-07 (PS 1.5)
TEST_F(Afferent, AFF07_SetFrontKeepsSides)
{
    obstacle_location_set_sides(true, true);
    obstacle_location_set_front(true);
    ExpectObstacle(true, true, true);
    obstacle_location_set_front(false);
    ExpectObstacle(false, true, true);
}

// AFF-08 (PS 1.6)
TEST_F(Afferent, AFF08_DustExistenceStoresValue)
{
    dust_existence_set(true);
    EXPECT_TRUE(dust_existence_get());
    dust_existence_set(false);
    EXPECT_FALSE(dust_existence_get());
}

// AFF-09 (PS 1, C2, C17, C20)
TEST_F(Afferent, AFF09_DetectOnTickSamplesEachOnceAndStores)
{
    front_sensor_on_interrupt(true);
    fake_port_set_left(true);
    fake_port_set_right(false);
    fake_port_set_dust(true);

    detect_on_tick();

    EXPECT_EQ(fake_port_left_reads(), 1);
    EXPECT_EQ(fake_port_right_reads(), 1);
    EXPECT_EQ(fake_port_dust_reads(), 1);
    ExpectObstacle(true, true, false); // front untouched by the Tick path
    EXPECT_TRUE(dust_existence_get());

    fake_port_set_left(false);
    fake_port_set_right(true);
    fake_port_set_dust(false);
    detect_on_tick();
    ExpectObstacle(true, false, true);
    EXPECT_FALSE(dust_existence_get());
    EXPECT_EQ(fake_port_left_reads(), 2);
}

} // namespace
