/**
 * @file sim_world.c
 * @brief Simulated environment (see sim_world.h).
 */
#include <stddef.h>
#include <string.h>

#include "sim_world.h"

static const char *const HEADING_NAMES[] = { "N", "E", "S", "W" };

const char *heading_name(Heading h)
{
    return ((int)h >= 0 && (int)h < 4) ? HEADING_NAMES[h] : "?";
}

bool heading_parse(const char *s, Heading *out)
{
    if (s == NULL || out == NULL) {
        return false;
    }
    for (int i = 0; i < 4; ++i) {
        if (strcmp(s, HEADING_NAMES[i]) == 0) {
            *out = (Heading)i;
            return true;
        }
    }
    return false;
}

static void heading_delta(Heading h, int *dx, int *dy)
{
    static const int DX[] = { 0, 1, 0, -1 };
    static const int DY[] = { -1, 0, 1, 0 };
    *dx = DX[h];
    *dy = DY[h];
}

static Heading heading_left(Heading h)
{
    return (Heading)(((int)h + 3) % 4);
}

static Heading heading_right(Heading h)
{
    return (Heading)(((int)h + 1) % 4);
}

bool world_blocked(const GridMap *map, int x, int y)
{
    if (x < 0 || x >= GRID_W || y < 0 || y >= GRID_H) {
        return true;
    }
    return (map != NULL) && map->obstacle[y][x];
}

static bool blocked_towards(const GridMap *map, Pose p, Heading h)
{
    int dx;
    int dy;
    heading_delta(h, &dx, &dy);
    return world_blocked(map, p.x + dx, p.y + dy);
}

SensorFrame world_sense(const GridMap *map, Pose pose)
{
    SensorFrame f;
    f.front = blocked_towards(map, pose, pose.dir);
    f.left = blocked_towards(map, pose, heading_left(pose.dir));
    f.right = blocked_towards(map, pose, heading_right(pose.dir));
    f.dust = (map != NULL) && map->dust[pose.y][pose.x];
    return f;
}

void motor_model_on_new_direction(MotorModel *motor)
{
    if (motor != NULL) {
        motor->turn_progress = 0;
    }
}

static void try_move(const GridMap *map, Pose *pose, Heading h, MotionResult *r)
{
    int dx;
    int dy;
    heading_delta(h, &dx, &dy);
    if (world_blocked(map, pose->x + dx, pose->y + dy)) {
        r->collision = true;
    } else {
        pose->x += dx;
        pose->y += dy;
    }
}

MotionResult world_apply_motion(const GridMap *map, Pose *pose, MotorModel *motor,
                                MotorCommand direction)
{
    MotionResult r;
    memset(&r, 0, sizeof r);
    if (pose == NULL || motor == NULL) {
        return r;
    }
    r.from = *pose;

    switch (direction) {
    case MOTOR_FORWARD:
        r.kind = MOTION_MOVE_FORWARD;
        try_move(map, pose, pose->dir, &r);
        break;
    case MOTOR_BACKWARD:
        r.kind = MOTION_MOVE_BACKWARD;
        try_move(map, pose, (Heading)(((int)pose->dir + 2) % 4), &r);
        break;
    case MOTOR_TURN_LEFT:
    case MOTOR_TURN_RIGHT:
        r.kind = (direction == MOTOR_TURN_LEFT) ? MOTION_TURNING_LEFT : MOTION_TURNING_RIGHT;
        motor->turn_progress += 1;
        if (motor->turn_progress >= TURN_TICKS_PER_90_DEG) {
            pose->dir = (direction == MOTOR_TURN_LEFT) ? heading_left(pose->dir)
                                                       : heading_right(pose->dir);
            motor->turn_progress = 0;
            r.rotated = true;
        }
        break;
    }
    r.turn_progress = motor->turn_progress;
    r.to = *pose;
    return r;
}

bool world_clean_cell(GridMap *map, int x, int y)
{
    if (map == NULL || x < 0 || x >= GRID_W || y < 0 || y >= GRID_H) {
        return false;
    }
    bool had = map->dust[y][x];
    map->dust[y][x] = false;
    return had;
}

int world_dust_count(const GridMap *map)
{
    int n = 0;
    if (map == NULL) {
        return 0;
    }
    for (int y = 0; y < GRID_H; ++y) {
        for (int x = 0; x < GRID_W; ++x) {
            n += map->dust[y][x] ? 1 : 0;
        }
    }
    return n;
}
