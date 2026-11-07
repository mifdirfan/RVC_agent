/**
 * @file clean_off.c
 * @brief Clean Off (Process 2.1.7): one-shot action (C25). Sends Cleaner Command = CLEAN_OFF
 *        once per Trigger.
 */
#include "clean_off.h"
#include "cleaner_interface.h"

void clean_off_trigger(void)
{
    cleaner_interface_send(CLEAN_OFF);
}
