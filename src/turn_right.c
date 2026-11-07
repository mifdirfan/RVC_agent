/**
 * @file turn_right.c
 * @brief Turn Right (Process 2.1.4): one-shot action (C25). Sends Motor Command = MOTOR_TURN_RIGHT
 *        once per Trigger.
 */
#include "turn_right.h"
#include "motor_interface.h"

void turn_right_trigger(void)
{
    motor_interface_send(MOTOR_TURN_RIGHT);
}
