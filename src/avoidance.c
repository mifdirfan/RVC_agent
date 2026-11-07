/**
 * @file avoidance.c
 * @brief Decide Avoidance: the left-first avoidance rule of PS 2.1
 *        DecideAvoidance(). Pure function, no state, no signal (C4, R3, R4).
 */
#include "avoidance.h"

AvoidChoice decide_avoidance(bool left, bool right)
{
    if (!left) {
        return AVOID_TURN_LEFT; /* left first (C4) */
    }
    if (!right) {
        return AVOID_TURN_RIGHT;
    }
    return AVOID_BACKWARD; /* both sides blocked (R4) */
}
