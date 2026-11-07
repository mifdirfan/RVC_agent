/**
 * @file left_sensor.c
 * @brief Left Sensor Interface (Process 1.2): reads Left Sensor Input once and returns it as Left Obstacle (C3).
 */
#include "left_sensor.h"
#include "rvc_port.h"

bool left_sensor_sample(void)
{
    return rvc_port_read_left_sensor();
}
