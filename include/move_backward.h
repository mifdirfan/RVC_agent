/**
 * @file move_backward.h
 * @brief Move Backward (Process 2.1.5): continuous action (C24).
 */
#ifndef MOVE_BACKWARD_H
#define MOVE_BACKWARD_H

#include "rvc_types.h"

#ifdef __cplusplus
extern "C" {
#endif

void move_backward_init(void);
/** Becomes active and sends Motor Command = Backward once. */
void move_backward_enable(void);
/** Becomes inactive; sends nothing. */
void move_backward_disable(void);

#ifdef __cplusplus
}
#endif

#endif /* MOVE_BACKWARD_H */
