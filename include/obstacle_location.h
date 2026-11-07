/**
 * @file obstacle_location.h
 * @brief Obstacle Location: Process 1.5 and its data store (information hiding).
 */
#ifndef OBSTACLE_LOCATION_H
#define OBSTACLE_LOCATION_H

#include "rvc_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Sets front, left and right to false (C21). */
void obstacle_location_init(void);
/** Updates only the front part (interrupt path, C1, C19). */
void obstacle_location_set_front(bool front);
/** Updates only the left and right parts (Tick path). */
void obstacle_location_set_sides(bool left, bool right);
/** Returns a snapshot of all three parts. */
ObstacleLocation obstacle_location_get(void);

#ifdef __cplusplus
}
#endif

#endif /* OBSTACLE_LOCATION_H */
