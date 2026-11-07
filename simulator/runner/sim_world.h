/**
 * @file sim_world.h
 * @brief Simulated environment: 10x10 grid map, RVC pose, sensor model and
 *        motion model. Contains NO control decisions; it only answers "what do
 *        the sensors see" and "what does the motor do with this Direction".
 *
 * Coordinates: x = 0..9 (column, left to right), y = 0..9 (row, top to bottom).
 * Heading N = towards y-1 (up on screen). Outside the grid counts as a wall.
 */
#ifndef SIM_WORLD_H
#define SIM_WORLD_H

#include <stdbool.h>
#include "rvc_types.h"

#define GRID_W 10
#define GRID_H 10
#define TURN_TICKS_PER_90_DEG 3 /* C5: a rotation lasts 3 ticks */

typedef enum { HEAD_N, HEAD_E, HEAD_S, HEAD_W } Heading;

typedef struct {
    int x;
    int y;
    Heading dir;
} Pose;

/** Obstacles and dust are kept independently, indexed [y][x]. */
typedef struct {
    bool obstacle[GRID_H][GRID_W];
    bool dust[GRID_H][GRID_W];
} GridMap;

/** Boolean sensor values for one tick (C3). */
typedef struct {
    bool front;
    bool left;
    bool right;
    bool dust;
} SensorFrame;

typedef enum {
    MOTION_MOVE_FORWARD,
    MOTION_MOVE_BACKWARD,
    MOTION_TURNING_LEFT,
    MOTION_TURNING_RIGHT
} MotionKind;

typedef struct {
    MotionKind kind;
    Pose from;
    Pose to;
    bool collision;     /* move into wall/obstacle was blocked */
    int turn_progress;  /* ticks of the current rotation done after this tick (0..2) */
    bool rotated;       /* heading changed by 90 degrees on this tick */
} MotionResult;

/** Motor (actuator) model state: progress of the rotation in progress. */
typedef struct {
    int turn_progress;
} MotorModel;

const char *heading_name(Heading h);
bool heading_parse(const char *s, Heading *out);

/** True when (x, y) is outside the grid or holds an obstacle. */
bool world_blocked(const GridMap *map, int x, int y);

/** Sensor values seen from pose: F/L/R relative to the heading, dust on the cell. */
SensorFrame world_sense(const GridMap *map, Pose pose);

/** Restarts rotation progress; called whenever a new Direction is written. */
void motor_model_on_new_direction(MotorModel *motor);

/**
 * Applies the Direction currently held by the motor for one tick:
 * Forward/Backward = 1 cell (blocked -> collision, pose unchanged);
 * Turn Left/Right = 1/3 of a 90-degree rotation, heading changes on the 3rd tick.
 */
MotionResult world_apply_motion(const GridMap *map, Pose *pose, MotorModel *motor,
                                MotorCommand direction);

/** Removes dust from (x, y). Returns true if there was dust. */
bool world_clean_cell(GridMap *map, int x, int y);

int world_dust_count(const GridMap *map);

#endif /* SIM_WORLD_H */
