/**
 * @file avoidance.h
 * @brief Decide Avoidance: left-first avoidance rule (PS 2.1 DecideAvoidance(), C4, R4).
 */
#ifndef AVOIDANCE_H
#define AVOIDANCE_H

#include "rvc_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Pure function: !left -> turn left; left && !right -> turn right; both -> backward. */
AvoidChoice decide_avoidance(bool left, bool right);

#ifdef __cplusplus
}
#endif

#endif /* AVOIDANCE_H */
