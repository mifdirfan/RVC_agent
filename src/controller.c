/**
 * @file controller.c
 * @brief Controller (Process 2.1.1): the Level 4 FSM (dfd_level_4_fsm.md, T1-T25).
 *
 * Holds the FSM state and the turn, reverse and power-up tick counters (all
 * hidden, SC Section 5.4). On each Tick it reads both stores, increments the
 * counter of the current timed state (saturating), tests the guards in FSM
 * priority order and issues Enable / Disable / Trigger only on a transition
 * (C26), in the FSM signal order (SC Section 7).
 */
#include <stdint.h>

#include "controller.h"
#include "obstacle_location.h"
#include "dust_existence.h"
#include "avoidance.h"
#include "move_forward.h"
#include "move_backward.h"
#include "turn_left.h"
#include "turn_right.h"
#include "clean_normal.h"
#include "clean_off.h"
#include "power_up_clean.h"

typedef enum {
    CTRL_FWD_NORMAL,
    CTRL_FWD_POWER_UP,
    CTRL_TURN_LEFT,
    CTRL_TURN_RIGHT,
    CTRL_MOVE_BACKWARD
} ControllerState;

enum {
    TURN_TICKS = 3,        /* C5 */
    REVERSE_MIN_TICKS = 3, /* C9 */
    POWER_UP_TICKS = 3,    /* C6 */
    COUNTER_LIMIT = 3      /* saturation point of every counter (SC 5.4) */
};

static ControllerState s_state;
static uint8_t s_turn_ticks;
static uint8_t s_reverse_ticks;
static uint8_t s_power_up_ticks;

/* Increment with saturation at COUNTER_LIMIT (SC 5.4). */
static uint8_t count_tick(uint8_t counter)
{
    return (counter < COUNTER_LIMIT) ? (uint8_t)(counter + 1u) : counter;
}

/* Movement signal of an avoidance choice, and entry into the matching state.
 * Shared by entry from a forward state, turn end and reverse end. */
static void start_avoidance(AvoidChoice choice)
{
    switch (choice) {
    case AVOID_TURN_LEFT:
        turn_left_trigger();
        s_turn_ticks = 0;
        s_state = CTRL_TURN_LEFT;
        break;
    case AVOID_TURN_RIGHT:
        turn_right_trigger();
        s_turn_ticks = 0;
        s_state = CTRL_TURN_RIGHT;
        break;
    case AVOID_BACKWARD:
        move_backward_enable();
        s_reverse_ticks = 0;
        s_state = CTRL_MOVE_BACKWARD;
        break;
    }
}

/* T2-T4, T7-T9: Disable "Move Forward"; Trigger "Clean Off"; movement signal. */
static void avoid_from_forward(ObstacleLocation obs)
{
    AvoidChoice choice = decide_avoidance(obs.left, obs.right);
    move_forward_disable();
    clean_off_trigger();
    start_avoidance(choice);
}

/* T1, T16, T21: Enable "Move Forward"; Trigger "Clean Normal". */
static void enter_forward_normal(void)
{
    move_forward_enable();
    clean_normal_trigger();
    s_state = CTRL_FWD_NORMAL;
}

static void on_fwd_normal(ObstacleLocation obs, bool dust)
{
    if (obs.front) {
        avoid_from_forward(obs);              /* T2, T3, T4 */
    } else if (dust) {
        power_up_clean_trigger();             /* T6 */
        s_power_up_ticks = 0;
        s_state = CTRL_FWD_POWER_UP;
    }
    /* else T5: stay, no signal (C13, C26) */
}

static void on_fwd_power_up(ObstacleLocation obs)
{
    s_power_up_ticks = count_tick(s_power_up_ticks);
    if (obs.front) {
        avoid_from_forward(obs);              /* T7, T8, T9 (C11) */
    } else if (s_power_up_ticks == POWER_UP_TICKS) {
        clean_normal_trigger();               /* T11 (C28) */
        s_state = CTRL_FWD_NORMAL;
    }
    /* else T10: stay; D is not tested (C14) */
}

static void on_turn(ObstacleLocation obs)
{
    s_turn_ticks = count_tick(s_turn_ticks);
    if (s_turn_ticks != TURN_TICKS) {
        return;                               /* T12, T17: F and D ignored */
    }
    if (obs.front) {
        /* T13, T14, T15 / T18, T19, T20 (C15) */
        start_avoidance(decide_avoidance(obs.left, obs.right));
    } else {
        enter_forward_normal();               /* T16, T21; D not tested (C29) */
    }
}

static void on_move_backward(ObstacleLocation obs)
{
    s_reverse_ticks = count_tick(s_reverse_ticks);
    if (s_reverse_ticks < REVERSE_MIN_TICKS) {
        return;                               /* T24 */
    }
    AvoidChoice choice = decide_avoidance(obs.left, obs.right); /* F not used (C15) */
    if (choice != AVOID_BACKWARD) {
        move_backward_disable();              /* T22, T23 */
        start_avoidance(choice);
    }
    /* else T25: keep reversing */
}

void controller_init(void)
{
    s_turn_ticks = 0;
    s_reverse_ticks = 0;
    s_power_up_ticks = 0;
    enter_forward_normal();                   /* T1 (C16, C27) */
}

void controller_tick(void)
{
    ObstacleLocation obs = obstacle_location_get();
    bool dust = dust_existence_get();

    switch (s_state) {
    case CTRL_FWD_NORMAL:
        on_fwd_normal(obs, dust);
        break;
    case CTRL_FWD_POWER_UP:
        on_fwd_power_up(obs);
        break;
    case CTRL_TURN_LEFT:
    case CTRL_TURN_RIGHT:
        on_turn(obs);
        break;
    case CTRL_MOVE_BACKWARD:
        on_move_backward(obs);
        break;
    }
}
