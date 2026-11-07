/**
 * @file cleaner_interface.c
 * @brief Cleaner Interface (Process 2.3) [library]. Sends Clean to the Cleaner
 *        only when the Cleaner Command differs from the last value sent, or when
 *        nothing has been sent yet (C12, C22, C27).
 */
#include "cleaner_interface.h"
#include "rvc_port.h"

static bool s_sent_any;
static CleanerCommand s_last_sent;

void cleaner_interface_init(void)
{
    s_sent_any = false; /* nothing sent yet (C27) */
    s_last_sent = CLEAN_ON;
}

void cleaner_interface_send(CleanerCommand cmd)
{
    if (!s_sent_any || cmd != s_last_sent) {
        s_sent_any = true;
        s_last_sent = cmd;
        rvc_port_write_clean(cmd);
    }
}
