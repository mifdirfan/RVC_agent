/**
 * @file rvc.c
 * @brief RVC System: start-up order (SC Section 3.2) and per-Tick order
 *        (SC Section 3.3). Single entry point for main(), tests and simulator.
 */
#include "rvc.h"
#include "obstacle_location.h"
#include "dust_existence.h"
#include "motor_interface.h"
#include "cleaner_interface.h"
#include "move_forward.h"
#include "move_backward.h"
#include "controller.h"
#include "detection.h"

void rvc_startup(void)
{
    obstacle_location_init();  /* 1: F = L = R = false (C21) */
    dust_existence_init();     /* 2: D = false (C21) */
    motor_interface_init();    /* 3: nothing sent yet (C27) */
    cleaner_interface_init();  /* 4: nothing sent yet (C27) */
    move_forward_init();       /* 5: inactive */
    move_backward_init();      /* 6: inactive */
    controller_init();         /* 7: FSM T1 -> Forward, clean_on (C22) */
}

void rvc_tick(void)
{
    detect_on_tick();          /* sensing first ... */
    controller_tick();         /* ... then the decision (C20) */
}
