/**
 * @file detection.h
 * @brief Detect Obstacle & Dust: afferent coordinator for the periodic sensors (Process 1, Tick part).
 */
#ifndef DETECTION_H
#define DETECTION_H

#include "rvc_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Samples left, right and dust once each and writes the stores (C2, C17). */
void detect_on_tick(void);

#ifdef __cplusplus
}
#endif

#endif /* DETECTION_H */
