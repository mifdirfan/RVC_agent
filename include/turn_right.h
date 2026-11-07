/**
 * @file turn_right.h
 * @brief Turn Right (Process 2.1.4): one-shot action (C25).
 */
#ifndef TURN_RIGHT_H
#define TURN_RIGHT_H

#include "rvc_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Sends Motor Command = Turn Right once per Trigger. */
void turn_right_trigger(void);

#ifdef __cplusplus
}
#endif

#endif /* TURN_RIGHT_H */
