/**
 * @file sim_ports.h
 * @brief Simulator side of the environment ports (rvc_port.h). The runner sets
 *        the sensor frame before each rvc_tick(); the controller's reads and
 *        writes are logged so the trace can show them per tick.
 */
#ifndef SIM_PORTS_H
#define SIM_PORTS_H

#include <stdbool.h>
#include "rvc_types.h"
#include "sim_world.h"

#define SIM_MAX_WRITES_PER_TICK 8

typedef struct {
    /* calls made by the controller during the current tick */
    int reads_left;
    int reads_right;
    int reads_dust;
    int n_dir_writes;
    MotorCommand dir_writes[SIM_MAX_WRITES_PER_TICK];
    int n_clean_writes;
    CleanerCommand clean_writes[SIM_MAX_WRITES_PER_TICK];
    /* values held by the actuators (last value ever written) */
    bool have_direction;
    MotorCommand direction;
    bool have_clean;
    CleanerCommand clean;
} SimPortLog;

/** Clears everything (new run). */
void sim_ports_reset(MotorModel *motor);
/** Clears the per-tick call log and installs this tick's sensor values. */
void sim_ports_begin_tick(SensorFrame frame);
const SimPortLog *sim_ports_log(void);

#endif /* SIM_PORTS_H */
