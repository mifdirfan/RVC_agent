/**
 * @file left_sensor.h
 * @brief Left Sensor Interface (Process 1.2).
 */
#ifndef LEFT_SENSOR_H
#define LEFT_SENSOR_H

#include "rvc_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Reads Left Sensor Input once and returns it as Left Obstacle (C3). */
bool left_sensor_sample(void);

#ifdef __cplusplus
}
#endif

#endif /* LEFT_SENSOR_H */
