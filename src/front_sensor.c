/**
 * @file front_sensor.c
 * @brief Front Sensor Interface (Process 1.1): interrupt entry (C1). Passes the
 *        Boolean value on unchanged, including false (C3, C19).
 */
#include "front_sensor.h"
#include "obstacle_location.h"

void front_sensor_on_interrupt(bool front_sensor_input)
{
    obstacle_location_set_front(front_sensor_input);
}
