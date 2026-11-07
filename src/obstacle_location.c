/**
 * @file obstacle_location.c
 * @brief Obstacle Location (Process 1.5 + data store). The store is hidden;
 *        the front part is written by the interrupt path, the sides on Tick.
 */
#include "obstacle_location.h"

static ObstacleLocation s_store;

void obstacle_location_init(void)
{
    s_store.front = false; /* C21 */
    s_store.left = false;
    s_store.right = false;
}

void obstacle_location_set_front(bool front)
{
    s_store.front = front;
}

void obstacle_location_set_sides(bool left, bool right)
{
    s_store.left = left;
    s_store.right = right;
}

ObstacleLocation obstacle_location_get(void)
{
    return s_store;
}
