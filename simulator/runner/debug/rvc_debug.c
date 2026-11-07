/**
 * @file rvc_debug.c
 * @brief Assembles the read-only debug view (rvc_debug.h).
 */
#include "rvc_debug.h"
#include "obstacle_location.h"
#include "dust_existence.h"

void rvc_debug_get(RvcDebugState *out)
{
    if (out == NULL) {
        return;
    }
    out->state = (DbgControllerState)rvc_dbg_controller_state();
    out->turn_ticks = rvc_dbg_turn_ticks();
    out->reverse_ticks = rvc_dbg_reverse_ticks();
    out->power_up_ticks = rvc_dbg_power_up_ticks();
    out->move_forward_active = rvc_dbg_move_forward_active();
    out->move_backward_active = rvc_dbg_move_backward_active();
    out->obstacle = obstacle_location_get();
    out->dust = dust_existence_get();
}
