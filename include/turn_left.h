/**
 * @file turn_left.h
 * @brief Turn Left (Process 2.1.3): one-shot action (C25).
 */
#ifndef TURN_LEFT_H
#define TURN_LEFT_H

#include "rvc_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Sends Motor Command = Turn Left once per Trigger. */
void turn_left_trigger(void);

#ifdef __cplusplus
}
#endif

#endif /* TURN_LEFT_H */
