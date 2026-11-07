/**
 * @file sim_runner.c
 * @brief RVC simulator runner. Links the approved src/ modules (not main.c) and
 *        drives them on a 10x10 grid.
 *
 * Usage: sim_runner <scenario.json> [trace.json]
 *   Writes the trace to trace.json, or to stdout when omitted.
 *   Exit code 0 = ok, 1 = I/O error, 2 = invalid scenario (JSON {"error": ...}).
 *
 * Tick k (1..max_ticks), one complete system update:
 *   1. Sense from the pose: F/L/R relative to the heading, dust on the cell.
 *   2. If F differs from the last front value delivered, raise the front
 *      interrupt: front_sensor_on_interrupt(F)  (C1, C19, C20).
 *   3. rvc_tick(): the real controller samples L/R/dust through the ports
 *      once (C17) and decides (C20). Port writes are logged.
 *   4. Cleaning: if the cleaner holds clean_on or clean_up, dust on the cell
 *      the RVC occupied during this tick is removed. clean_off removes nothing.
 *   5. Motion: the Direction held by the motor is applied (sim_world.c).
 *   6. A full snapshot is written.
 * Tick 0 is the state right after rvc_startup() (C21, C22, C27).
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "rvc.h"
#include "front_sensor.h"
#include "rvc_debug.h"
#include "sim_ports.h"
#include "sim_scenario.h"
#include "sim_world.h"

#define MAX_INPUT_BYTES (256 * 1024)

static const char *state_name(DbgControllerState s)
{
    switch (s) {
    case DBG_FWD_NORMAL: return "FwdNormal";
    case DBG_FWD_POWER_UP: return "FwdPowerUp";
    case DBG_TURN_LEFT: return "TurnLeft";
    case DBG_TURN_RIGHT: return "TurnRight";
    case DBG_MOVE_BACKWARD: return "MoveBackward";
    }
    return "?";
}

static const char *motor_name(MotorCommand m)
{
    switch (m) {
    case MOTOR_FORWARD: return "Forward";
    case MOTOR_BACKWARD: return "Backward";
    case MOTOR_TURN_LEFT: return "Turn Left";
    case MOTOR_TURN_RIGHT: return "Turn Right";
    }
    return "?";
}

static const char *clean_name(CleanerCommand c)
{
    switch (c) {
    case CLEAN_ON: return "clean_on";
    case CLEAN_OFF: return "clean_off";
    case CLEAN_UP: return "clean_up";
    }
    return "?";
}

static const char *motion_name(MotionKind k)
{
    switch (k) {
    case MOTION_MOVE_FORWARD: return "move_forward";
    case MOTION_MOVE_BACKWARD: return "move_backward";
    case MOTION_TURNING_LEFT: return "turning_left";
    case MOTION_TURNING_RIGHT: return "turning_right";
    }
    return "?";
}

/*
 * Names the FSM row (dfd_level_4_fsm.md) that matches the OBSERVED change of
 * the controller's state and counters. It is an annotation for the evaluator
 * only; it never feeds back into the simulation and decides nothing.
 */
static const char *fsm_row(const RvcDebugState *b, const RvcDebugState *a)
{
    switch (b->state) {
    case DBG_FWD_NORMAL:
        switch (a->state) {
        case DBG_TURN_LEFT: return "T2";
        case DBG_TURN_RIGHT: return "T3";
        case DBG_MOVE_BACKWARD: return "T4";
        case DBG_FWD_NORMAL: return "T5";
        case DBG_FWD_POWER_UP: return "T6";
        }
        break;
    case DBG_FWD_POWER_UP:
        switch (a->state) {
        case DBG_TURN_LEFT: return "T7";
        case DBG_TURN_RIGHT: return "T8";
        case DBG_MOVE_BACKWARD: return "T9";
        case DBG_FWD_POWER_UP: return "T10";
        case DBG_FWD_NORMAL: return "T11";
        }
        break;
    case DBG_TURN_LEFT:
        switch (a->state) {
        case DBG_TURN_LEFT: return (a->turn_ticks == 0) ? "T13" : "T12";
        case DBG_TURN_RIGHT: return "T14";
        case DBG_MOVE_BACKWARD: return "T15";
        case DBG_FWD_NORMAL: return "T16";
        case DBG_FWD_POWER_UP: return "?";
        }
        break;
    case DBG_TURN_RIGHT:
        switch (a->state) {
        case DBG_TURN_RIGHT: return (a->turn_ticks == 0) ? "T18" : "T17";
        case DBG_TURN_LEFT: return "T19";
        case DBG_MOVE_BACKWARD: return "T20";
        case DBG_FWD_NORMAL: return "T21";
        case DBG_FWD_POWER_UP: return "?";
        }
        break;
    case DBG_MOVE_BACKWARD:
        switch (a->state) {
        case DBG_TURN_LEFT: return "T22";
        case DBG_TURN_RIGHT: return "T23";
        case DBG_MOVE_BACKWARD: return (a->reverse_ticks < 3) ? "T24" : "T25";
        default: return "?";
        }
    }
    return "?";
}

static const char *jbool(bool b)
{
    return b ? "true" : "false";
}

static void write_pose(FILE *o, const char *key, Pose p)
{
    fprintf(o, "\"%s\":{\"x\":%d,\"y\":%d,\"dir\":\"%s\"}", key, p.x, p.y, heading_name(p.dir));
}

static void write_cells(FILE *o, const char *key, const GridMap *m, bool dust)
{
    bool first = true;
    fprintf(o, "\"%s\":[", key);
    for (int y = 0; y < GRID_H; ++y) {
        for (int x = 0; x < GRID_W; ++x) {
            bool set = dust ? m->dust[y][x] : m->obstacle[y][x];
            if (set) {
                fprintf(o, "%s[%d,%d]", first ? "" : ",", x, y);
                first = false;
            }
        }
    }
    fputc(']', o);
}

static void write_controller(FILE *o, const RvcDebugState *d)
{
    fprintf(o,
            "\"controller\":{\"state\":\"%s\",\"turn_ticks\":%u,\"reverse_ticks\":%u,"
            "\"power_up_ticks\":%u,\"move_forward_active\":%s,\"move_backward_active\":%s},"
            "\"stores\":{\"front\":%s,\"left\":%s,\"right\":%s,\"dust\":%s}",
            state_name(d->state), (unsigned)d->turn_ticks, (unsigned)d->reverse_ticks,
            (unsigned)d->power_up_ticks, jbool(d->move_forward_active),
            jbool(d->move_backward_active), jbool(d->obstacle.front), jbool(d->obstacle.left),
            jbool(d->obstacle.right), jbool(d->dust));
}

static void write_port_calls(FILE *o, const SimPortLog *log)
{
    fprintf(o, "\"port_calls\":{\"read_left\":%d,\"read_right\":%d,\"read_dust\":%d,\"write_direction\":[",
            log->reads_left, log->reads_right, log->reads_dust);
    for (int i = 0; i < log->n_dir_writes && i < SIM_MAX_WRITES_PER_TICK; ++i) {
        fprintf(o, "%s\"%s\"", i ? "," : "", motor_name(log->dir_writes[i]));
    }
    fputs("],\"write_clean\":[", o);
    for (int i = 0; i < log->n_clean_writes && i < SIM_MAX_WRITES_PER_TICK; ++i) {
        fprintf(o, "%s\"%s\"", i ? "," : "", clean_name(log->clean_writes[i]));
    }
    fprintf(o, "]},\"outputs\":{\"motor\":%s%s%s,\"cleaner\":%s%s%s}",
            log->have_direction ? "\"" : "", log->have_direction ? motor_name(log->direction) : "null",
            log->have_direction ? "\"" : "",
            log->have_clean ? "\"" : "", log->have_clean ? clean_name(log->clean) : "null",
            log->have_clean ? "\"" : "");
}

static char *read_file(const char *path)
{
    FILE *f = fopen(path, "rb");
    if (f == NULL) {
        return NULL;
    }
    char *buf = malloc(MAX_INPUT_BYTES + 1);
    if (buf == NULL) {
        fclose(f);
        return NULL;
    }
    size_t n = fread(buf, 1, MAX_INPUT_BYTES, f);
    fclose(f);
    buf[n] = '\0';
    return buf;
}

static void json_escape_print(FILE *o, const char *s)
{
    for (; *s != '\0'; ++s) {
        if (*s == '"' || *s == '\\') {
            fputc('\\', o);
        }
        fputc((*s >= 0x20) ? *s : ' ', o);
    }
}

static void run(FILE *o, Scenario *sc)
{
    GridMap map = sc->map;
    Pose pose = sc->start;
    MotorModel motor = { 0 };
    bool last_front = false; /* front value held by the store after start-up (C21) */
    RvcDebugState before;
    RvcDebugState after;

    fputs("{\"format\":\"rvc-trace/1\",\"scenario\":{", o);
    write_pose(o, "start", sc->start);
    fputc(',', o);
    write_cells(o, "obstacles", &sc->map, false);
    fputc(',', o);
    write_cells(o, "dust", &sc->map, true);
    fprintf(o, ",\"max_ticks\":%d},\"ticks\":[", sc->max_ticks);

    /* Tick 0: start-up */
    sim_ports_reset(&motor);
    sim_ports_begin_tick((SensorFrame){ false, false, false, false });
    rvc_startup();
    rvc_debug_get(&after);
    fputs("{\"tick\":0,\"phase\":\"startup\",", o);
    write_pose(o, "pose", pose);
    fputs(",\"sensors\":null,\"front_interrupt\":null,", o);
    write_controller(o, &after);
    fputc(',', o);
    write_port_calls(o, sim_ports_log());
    fprintf(o, ",\"transition\":{\"from\":null,\"to\":\"%s\",\"fsm\":\"T1\",\"changed\":true},"
               "\"motion\":null,\"cleaned\":null,",
            state_name(after.state));
    write_cells(o, "dust", &map, true);
    fputc('}', o);

    for (int k = 1; k <= sc->max_ticks; ++k) {
        Pose sensed_at = pose;
        SensorFrame frame = world_sense(&map, pose);
        bool interrupt = (frame.front != last_front);

        rvc_debug_get(&before);
        sim_ports_begin_tick(frame);
        if (interrupt) {
            front_sensor_on_interrupt(frame.front);
            last_front = frame.front;
        }
        rvc_tick();
        rvc_debug_get(&after);
        const SimPortLog *log = sim_ports_log();

        bool cleaning = log->have_clean && (log->clean == CLEAN_ON || log->clean == CLEAN_UP);
        bool cleaned = cleaning && world_clean_cell(&map, pose.x, pose.y);

        MotionResult mr;
        memset(&mr, 0, sizeof mr);
        bool moved = log->have_direction;
        if (moved) {
            mr = world_apply_motion(&map, &pose, &motor, log->direction);
        }

        fprintf(o, ",{\"tick\":%d,\"phase\":\"tick\",", k);
        write_pose(o, "sensed_at", sensed_at);
        fputc(',', o);
        write_pose(o, "pose", pose);
        fprintf(o, ",\"sensors\":{\"front\":%s,\"left\":%s,\"right\":%s,\"dust\":%s},",
                jbool(frame.front), jbool(frame.left), jbool(frame.right), jbool(frame.dust));
        if (interrupt) {
            fprintf(o, "\"front_interrupt\":%s,", jbool(frame.front));
        } else {
            fputs("\"front_interrupt\":null,", o);
        }
        write_controller(o, &after);
        fputc(',', o);
        write_port_calls(o, log);
        fprintf(o, ",\"transition\":{\"from\":\"%s\",\"to\":\"%s\",\"fsm\":\"%s\",\"changed\":%s},",
                state_name(before.state), state_name(after.state), fsm_row(&before, &after),
                jbool(before.state != after.state ||
                      (after.state == before.state &&
                       (after.state == DBG_TURN_LEFT || after.state == DBG_TURN_RIGHT) &&
                       after.turn_ticks == 0)));
        if (moved) {
            fprintf(o, "\"motion\":{\"kind\":\"%s\",\"collision\":%s,\"turn_progress\":%d,\"rotated\":%s},",
                    motion_name(mr.kind), jbool(mr.collision), mr.turn_progress, jbool(mr.rotated));
        } else {
            fputs("\"motion\":null,", o);
        }
        if (cleaned) {
            fprintf(o, "\"cleaned\":[%d,%d],", sensed_at.x, sensed_at.y);
        } else {
            fputs("\"cleaned\":null,", o);
        }
        write_cells(o, "dust", &map, true);
        fputc('}', o);
    }
    fprintf(o, "],\"summary\":{\"ticks_run\":%d,\"dust_remaining\":%d,", sc->max_ticks,
            world_dust_count(&map));
    write_pose(o, "final_pose", pose);
    fputs("}}\n", o);
}

int main(int argc, char **argv)
{
    if (argc < 2) {
        fprintf(stderr, "usage: sim_runner <scenario.json> [trace.json]\n");
        return 1;
    }
    char *json = read_file(argv[1]);
    if (json == NULL) {
        fprintf(stderr, "cannot read %s\n", argv[1]);
        return 1;
    }
    static Scenario sc;
    char err[256];
    if (!scenario_parse(json, &sc, err, sizeof err)) {
        free(json);
        fputs("{\"error\":\"", stdout);
        json_escape_print(stdout, err);
        fputs("\"}\n", stdout);
        return 2;
    }
    free(json);

    FILE *o = stdout;
    if (argc >= 3) {
        o = fopen(argv[2], "wb");
        if (o == NULL) {
            fprintf(stderr, "cannot write %s\n", argv[2]);
            return 1;
        }
    }
    run(o, &sc);
    if (o != stdout) {
        fclose(o);
    }
    return 0;
}
