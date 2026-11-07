/**
 * @file front_sensor.h
 * @brief Front Sensor Interface (Process 1.1): interrupt entry (C1).
 */
#ifndef FRONT_SENSOR_H
#define FRONT_SENSOR_H

#include "rvc_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Called by the platform on the front interrupt. Passes the value on unchanged (C3, C19). */
void front_sensor_on_interrupt(bool front_sensor_input);

#ifdef __cplusplus
}
#endif

#endif /* FRONT_SENSOR_H */
