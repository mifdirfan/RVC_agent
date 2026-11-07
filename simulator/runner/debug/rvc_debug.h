/**
 * @file rvc_debug.h
 * @brief Read-only debug view of hidden RVC module state (simulator only).
 *
 * The Controller FSM state, its three tick counters and the active flags of the
 * two continuous motor processes are hidden (static) in src/ (SC Section 5.4)
 * and are not observable through the environment ports. rvc_debug_get() copies
 * them out without changing them. src/ is not modified: each dbg_*.c file
 * compiles the unmodified src/<module>.c in its own translation unit and adds a
 * getter (see dbg_controller.c).
 */
#ifndef RVC_DEBUG_H
#define RVC_DEBUG_H

#include <stdint.h>
#include "rvc_types.h"

/** Mirrors the private ControllerState of controller.c (same order). */
typedef enum {
    DBG_FWD_NORMAL,
    DBG_FWD_POWER_UP,
    DBG_TURN_LEFT,
    DBG_TURN_RIGHT,
    DBG_MOVE_BACKWARD
} DbgControllerState;

typedef struct {
    DbgControllerState state;   /* controller.c s_state */
    uint8_t turn_ticks;         /* controller.c s_turn_ticks */
    uint8_t reverse_ticks;      /* controller.c s_reverse_ticks */
    uint8_t power_up_ticks;     /* controller.c s_power_up_ticks */
    bool move_forward_active;   /* move_forward.c s_active */
    bool move_backward_active;  /* move_backward.c s_active */
    ObstacleLocation obstacle;  /* obstacle_location_get() */
    bool dust;                  /* dust_existence_get() */
} RvcDebugState;

/** Copies the current hidden state into *out. Does nothing if out is NULL. */
void rvc_debug_get(RvcDebugState *out);

/* Per-module getters, defined in dbg_*.c. */
int rvc_dbg_controller_state(void);
uint8_t rvc_dbg_turn_ticks(void);
uint8_t rvc_dbg_reverse_ticks(void);
uint8_t rvc_dbg_power_up_ticks(void);
bool rvc_dbg_move_forward_active(void);
bool rvc_dbg_move_backward_active(void);

#endif /* RVC_DEBUG_H */
