/**
 * @file dbg_controller.c
 * @brief Compiles the unmodified src/controller.c and exposes its hidden state
 *        read-only. The simulator links this file INSTEAD of src/controller.c,
 *        so the controller code that runs is byte-for-byte the approved one.
 */
#include "controller.c" /* src/ is on the include path; file is not modified */
#include "rvc_debug.h"

int rvc_dbg_controller_state(void)
{
    return (int)s_state;
}

uint8_t rvc_dbg_turn_ticks(void)
{
    return s_turn_ticks;
}

uint8_t rvc_dbg_reverse_ticks(void)
{
    return s_reverse_ticks;
}

uint8_t rvc_dbg_power_up_ticks(void)
{
    return s_power_up_ticks;
}
