/**
 * @file right_sensor.c
 * @brief Right Sensor Interface (Process 1.3): reads Right Sensor Input once and returns it as Right Obstacle (C3).
 */
#include "right_sensor.h"
#include "rvc_port.h"

bool right_sensor_sample(void)
{
    return rvc_port_read_right_sensor();
}
