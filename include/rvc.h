/**
 * @file rvc.h
 * @brief RVC System: start-up order and per-Tick order (SC Section 3.2, 3.3).
 */
#ifndef RVC_H
#define RVC_H

#include "rvc_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Start-up steps 1-7 (C21, C27, T1). Call once before the first Tick. */
void rvc_startup(void);
/** One Tick: detect_on_tick() then controller_tick() (C20). */
void rvc_tick(void);

#ifdef __cplusplus
}
#endif

#endif /* RVC_H */
