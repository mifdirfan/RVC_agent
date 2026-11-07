/**
 * @file right_sensor.h
 * @brief Right Sensor Interface (Process 1.3).
 */
#ifndef RIGHT_SENSOR_H
#define RIGHT_SENSOR_H

#include "rvc_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Reads Right Sensor Input once and returns it as Right Obstacle (C3). */
bool right_sensor_sample(void);

#ifdef __cplusplus
}
#endif

#endif /* RIGHT_SENSOR_H */
