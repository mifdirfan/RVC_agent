/**
 * @file dust_sensor.h
 * @brief Dust Sensor Interface (Process 1.4).
 */
#ifndef DUST_SENSOR_H
#define DUST_SENSOR_H

#include "rvc_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Reads Dust Sensor Input once and returns it as Dust (C3). */
bool dust_sensor_sample(void);

#ifdef __cplusplus
}
#endif

#endif /* DUST_SENSOR_H */
