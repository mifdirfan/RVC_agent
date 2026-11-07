/**
 * @file clean_off.h
 * @brief Clean Off (Process 2.1.7): one-shot action (C25).
 */
#ifndef CLEAN_OFF_H
#define CLEAN_OFF_H

#include "rvc_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Sends Cleaner Command = clean_off once per Trigger. */
void clean_off_trigger(void);

#ifdef __cplusplus
}
#endif

#endif /* CLEAN_OFF_H */
