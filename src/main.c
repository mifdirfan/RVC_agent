/**
 * @file main.c
 * @brief Main: root module. Start-up once, then one rvc_tick() per Tick.
 *        The requirements define no shutdown, so the loop never ends (R7).
 */
#include "rvc.h"
#include "rvc_port.h"

int main(void)
{
    rvc_startup();
    for (;;) {
        rvc_port_wait_tick();
        rvc_tick();
    }
}
