#include "fake_interfaces.h"
#include "motor_interface.h"
#include "cleaner_interface.h"

#define MAX_CALLS 256
static InterfaceCall s_log[MAX_CALLS];
static size_t s_count;

void fake_interfaces_reset(void) { s_count = 0; }
size_t fake_interfaces_count(void) { return s_count; }
InterfaceCall fake_interfaces_at(size_t i)
{
    InterfaceCall none = { IF_MOTOR, -1 };
    return (i < s_count) ? s_log[i] : none;
}
static void record(InterfaceKind k, int v)
{
    if (s_count < MAX_CALLS) {
        s_log[s_count].kind = k;
        s_log[s_count].value = v;
        s_count++;
    }
}

void motor_interface_init(void) {}
void motor_interface_send(MotorCommand cmd) { record(IF_MOTOR, (int)cmd); }
void cleaner_interface_init(void) {}
void cleaner_interface_send(CleanerCommand cmd) { record(IF_CLEANER, (int)cmd); }
