/**
 * @file power_up_clean.c
 * @brief Power-Up Clean (Process 2.1.8): one-shot action (C25). Sends Cleaner Command = CLEAN_UP
 *        once per Trigger.
 */
#include "power_up_clean.h"
#include "cleaner_interface.h"

void power_up_clean_trigger(void)
{
    cleaner_interface_send(CLEAN_UP);
}
