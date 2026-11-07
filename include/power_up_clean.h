/**
 * @file power_up_clean.h
 * @brief Power-Up Clean (Process 2.1.8): one-shot action (C25).
 */
#ifndef POWER_UP_CLEAN_H
#define POWER_UP_CLEAN_H

#include "rvc_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Sends Cleaner Command = clean_up once per Trigger. */
void power_up_clean_trigger(void);

#ifdef __cplusplus
}
#endif

#endif /* POWER_UP_CLEAN_H */
