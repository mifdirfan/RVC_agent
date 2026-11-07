/**
 * @file motor_interface.h
 * @brief Motor Interface (Process 2.2) [library]: sends Direction only on change (C12).
 */
#ifndef MOTOR_INTERFACE_H
#define MOTOR_INTERFACE_H

#include "rvc_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Nothing sent yet (C27). */
void motor_interface_init(void);
/** Writes Direction if nothing was sent yet or cmd differs from the last value sent. */
void motor_interface_send(MotorCommand cmd);

#ifdef __cplusplus
}
#endif

#endif /* MOTOR_INTERFACE_H */
