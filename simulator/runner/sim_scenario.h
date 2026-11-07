/**
 * @file sim_scenario.h
 * @brief Scenario input: parsing and validation of the scenario JSON.
 *
 * Accepted JSON (other keys are ignored):
 *   { "start": {"x": 4, "y": 4, "dir": "N"},
 *     "obstacles": [[x, y], ...],
 *     "dust": [[x, y], ...],
 *     "max_ticks": 100 }
 */
#ifndef SIM_SCENARIO_H
#define SIM_SCENARIO_H

#include <stddef.h>
#include "sim_world.h"

#define SIM_TICK_LIMIT 100

typedef struct {
    Pose start;
    GridMap map;
    int max_ticks;
} Scenario;

/**
 * Parses and validates json. On failure returns false and writes a message to
 * err (never modifies the map silently: every problem is rejected).
 */
bool scenario_parse(const char *json, Scenario *out, char *err, size_t err_len);

#endif /* SIM_SCENARIO_H */
