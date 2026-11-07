/**
 * @file rvc_types.h
 * @brief Shared value types from the Data Dictionary (SC Section 5.3).
 */
#ifndef RVC_TYPES_H
#define RVC_TYPES_H

#include <stdbool.h>

/** Obstacle Location = Front Obstacle + Left Obstacle + Right Obstacle (true = blocked). */
typedef struct {
    bool front;
    bool left;
    bool right;
} ObstacleLocation;

/** Motor Command; also used for Direction (1:1 mapping, DD 4.4). */
typedef enum {
    MOTOR_FORWARD,
    MOTOR_BACKWARD,
    MOTOR_TURN_LEFT,
    MOTOR_TURN_RIGHT
} MotorCommand;

/** Cleaner Command; also used for Clean (1:1 mapping, DD 4.4, C7). */
typedef enum {
    CLEAN_ON,
    CLEAN_OFF,
    CLEAN_UP
} CleanerCommand;

/** Avoidance choice of PS 2.1 DecideAvoidance() (Controller-internal design value). */
typedef enum {
    AVOID_TURN_LEFT,
    AVOID_TURN_RIGHT,
    AVOID_BACKWARD
} AvoidChoice;

#endif /* RVC_TYPES_H */
