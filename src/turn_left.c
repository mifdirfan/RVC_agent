/**
 * @file turn_left.c
 * @brief Turn Left (Process 2.1.3): one-shot action (C25). Sends Motor Command = MOTOR_TURN_LEFT
 *        once per Trigger.
 */
#include "turn_left.h"
#include "motor_interface.h"

void turn_left_trigger(void)
{
    motor_interface_send(MOTOR_TURN_LEFT);
}
