/**
 * @file sim_ports.c
 * @brief Implements the six rvc_port_* functions of rvc_port.h from the grid.
 */
#include <string.h>

#include "rvc_port.h"
#include "sim_ports.h"

static SensorFrame s_frame;
static SimPortLog s_log;
static MotorModel *s_motor; /* rotation restarts on every new Direction */

void sim_ports_reset(MotorModel *motor)
{
    memset(&s_frame, 0, sizeof s_frame);
    memset(&s_log, 0, sizeof s_log);
    s_motor = motor;
}

void sim_ports_begin_tick(SensorFrame frame)
{
    s_frame = frame;
    s_log.reads_left = 0;
    s_log.reads_right = 0;
    s_log.reads_dust = 0;
    s_log.n_dir_writes = 0;
    s_log.n_clean_writes = 0;
}

const SimPortLog *sim_ports_log(void)
{
    return &s_log;
}

bool rvc_port_read_left_sensor(void)
{
    s_log.reads_left++;
    return s_frame.left;
}

bool rvc_port_read_right_sensor(void)
{
    s_log.reads_right++;
    return s_frame.right;
}

bool rvc_port_read_dust_sensor(void)
{
    s_log.reads_dust++;
    return s_frame.dust;
}

void rvc_port_wait_tick(void)
{
    /* The runner drives the ticks itself; main.c is not linked. */
}

void rvc_port_write_direction(MotorCommand direction)
{
    if (s_log.n_dir_writes < SIM_MAX_WRITES_PER_TICK) {
        s_log.dir_writes[s_log.n_dir_writes] = direction;
    }
    s_log.n_dir_writes++;
    s_log.have_direction = true;
    s_log.direction = direction;
    motor_model_on_new_direction(s_motor);
}

void rvc_port_write_clean(CleanerCommand clean)
{
    if (s_log.n_clean_writes < SIM_MAX_WRITES_PER_TICK) {
        s_log.clean_writes[s_log.n_clean_writes] = clean;
    }
    s_log.n_clean_writes++;
    s_log.have_clean = true;
    s_log.clean = clean;
}
