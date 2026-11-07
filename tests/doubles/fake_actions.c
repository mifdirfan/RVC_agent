#include "fake_actions.h"
#include "move_forward.h"
#include "move_backward.h"
#include "turn_left.h"
#include "turn_right.h"
#include "clean_normal.h"
#include "clean_off.h"
#include "power_up_clean.h"

#define MAX_SIGNALS 1024
static Signal s_log[MAX_SIGNALS];
static size_t s_count;

void fake_actions_reset(void) { s_count = 0; }
size_t fake_actions_count(void) { return s_count; }
Signal fake_actions_at(size_t i) { return (i < s_count) ? s_log[i] : (Signal)-1; }
static void record(Signal s)
{
    if (s_count < MAX_SIGNALS) {
        s_log[s_count++] = s;
    }
}

void move_forward_init(void) {}
void move_forward_enable(void) { record(SIG_MF_ENABLE); }
void move_forward_disable(void) { record(SIG_MF_DISABLE); }
void move_backward_init(void) {}
void move_backward_enable(void) { record(SIG_MB_ENABLE); }
void move_backward_disable(void) { record(SIG_MB_DISABLE); }
void turn_left_trigger(void) { record(SIG_TL); }
void turn_right_trigger(void) { record(SIG_TR); }
void clean_normal_trigger(void) { record(SIG_CN); }
void clean_off_trigger(void) { record(SIG_CO); }
void power_up_clean_trigger(void) { record(SIG_PU); }
