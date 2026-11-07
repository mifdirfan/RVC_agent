/**
 * @file move_forward.c
 * @brief Move Forward (Process 2.1.2): continuous action (C24). On Enable it becomes
 *        active and sends Motor Command = MOTOR_FORWARD once; on Disable it becomes
 *        inactive and sends nothing.
 */
#include "move_forward.h"
#include "motor_interface.h"

static bool s_active;

void move_forward_init(void)
{
    s_active = false;
}

void move_forward_enable(void)
{
    s_active = true;
    motor_interface_send(MOTOR_FORWARD);
}

void move_forward_disable(void)
{
    s_active = false; /* no command on Disable */
}
