/**
 * @file detection.c
 * @brief Detect Obstacle & Dust (Process 1, Tick part): samples the periodic
 *        sensors once each and writes the stores. Interprets nothing (C2, C17).
 */
#include "detection.h"
#include "left_sensor.h"
#include "right_sensor.h"
#include "dust_sensor.h"
#include "obstacle_location.h"
#include "dust_existence.h"

void detect_on_tick(void)
{
    bool left = left_sensor_sample();
    bool right = right_sensor_sample();
    obstacle_location_set_sides(left, right);

    bool dust = dust_sensor_sample();
    dust_existence_set(dust);
}
