/**
 * @file controller.h
 * @brief Controller (Process 2.1.1): the Level 4 FSM, T1-T25.
 */
#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "rvc_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Runs FSM T1: Enable Move Forward, Trigger Clean Normal (C27). */
void controller_init(void);
/** Handles one Tick, after sensing (C20). Signals only on a transition (C26). */
void controller_tick(void);

#ifdef __cplusplus
}
#endif

#endif /* CONTROLLER_H */
