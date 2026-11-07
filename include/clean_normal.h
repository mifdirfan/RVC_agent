/**
 * @file clean_normal.h
 * @brief Clean Normal (Process 2.1.6): one-shot action (C25).
 */
#ifndef CLEAN_NORMAL_H
#define CLEAN_NORMAL_H

#include "rvc_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Sends Cleaner Command = clean_on once per Trigger. */
void clean_normal_trigger(void);

#ifdef __cplusplus
}
#endif

#endif /* CLEAN_NORMAL_H */
