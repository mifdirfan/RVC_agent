/**
 * @file cleaner_interface.h
 * @brief Cleaner Interface (Process 2.3) [library]: sends Clean only on change (C12).
 */
#ifndef CLEANER_INTERFACE_H
#define CLEANER_INTERFACE_H

#include "rvc_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Nothing sent yet (C27). */
void cleaner_interface_init(void);
/** Writes Clean if nothing was sent yet or cmd differs from the last value sent. */
void cleaner_interface_send(CleanerCommand cmd);

#ifdef __cplusplus
}
#endif

#endif /* CLEANER_INTERFACE_H */
