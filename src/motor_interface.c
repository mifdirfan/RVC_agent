/**
 * @file motor_interface.c
 * @brief Motor Interface (Process 2.2) [library]. Sends Direction to the Motor
 *        only when the Motor Command differs from the last value sent, or when
 *        nothing has been sent yet (C12, C22, C27).
 */
#include "motor_interface.h"
#include "rvc_port.h"

static bool s_sent_any;
static MotorCommand s_last_sent;

void motor_interface_init(void)
{
    s_sent_any = false; /* nothing sent yet (C27) */
    s_last_sent = MOTOR_FORWARD;
}

void motor_interface_send(MotorCommand cmd)
{
    if (!s_sent_any || cmd != s_last_sent) {
        s_sent_any = true;
        s_last_sent = cmd;
        rvc_port_write_direction(cmd);
    }
}
