/**
 * @file clean_normal.c
 * @brief Clean Normal (Process 2.1.6): one-shot action (C25). Sends Cleaner Command = CLEAN_ON
 *        once per Trigger.
 */
#include "clean_normal.h"
#include "cleaner_interface.h"

void clean_normal_trigger(void)
{
    cleaner_interface_send(CLEAN_ON);
}
