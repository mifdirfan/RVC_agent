/**
 * @file move_forward.h
 * @brief Move Forward (Process 2.1.2): continuous action (C24).
 */
#ifndef MOVE_FORWARD_H
#define MOVE_FORWARD_H

#include "rvc_types.h"

#ifdef __cplusplus
extern "C" {
#endif

void move_forward_init(void);
/** Becomes active and sends Motor Command = Forward once. */
void move_forward_enable(void);
/** Becomes inactive; sends nothing. */
void move_forward_disable(void);

#ifdef __cplusplus
}
#endif

#endif /* MOVE_FORWARD_H */
