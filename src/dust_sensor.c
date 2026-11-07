/**
 * @file dust_sensor.c
 * @brief Dust Sensor Interface (Process 1.4): reads Dust Sensor Input once and returns it as Dust (C3).
 */
#include "dust_sensor.h"
#include "rvc_port.h"

bool dust_sensor_sample(void)
{
    return rvc_port_read_dust_sensor();
}
