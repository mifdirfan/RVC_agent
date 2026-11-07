/**
 * @file move_backward.c
 * @brief Move Backward (Process 2.1.5): continuous action (C24). On Enable it becomes
 *        active and sends Motor Command = MOTOR_BACKWARD once; on Disable it becomes
 *        inactive and sends nothing.
 */
#include "move_backward.h"
#include "motor_interface.h"

static bool s_active;

void move_backward_init(void)
{
    s_active = false;
}

void move_backward_enable(void)
{
    s_active = true;
    motor_interface_send(MOTOR_BACKWARD);
}

void move_backward_disable(void)
{
    s_active = false; /* no command on Disable */
}
