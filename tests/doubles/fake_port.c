#include "fake_port.h"
#include "rvc_port.h"

static bool s_left, s_right, s_dust;
static int s_left_reads, s_right_reads, s_dust_reads, s_wait_calls;
static PortWrite s_log[FAKE_PORT_MAX_WRITES];
static size_t s_count;

void fake_port_reset(void)
{
    s_left = s_right = s_dust = false;
    s_left_reads = s_right_reads = s_dust_reads = s_wait_calls = 0;
    s_count = 0;
}
void fake_port_set_left(bool v) { s_left = v; }
void fake_port_set_right(bool v) { s_right = v; }
void fake_port_set_dust(bool v) { s_dust = v; }
int fake_port_left_reads(void) { return s_left_reads; }
int fake_port_right_reads(void) { return s_right_reads; }
int fake_port_dust_reads(void) { return s_dust_reads; }
int fake_port_wait_calls(void) { return s_wait_calls; }
size_t fake_port_write_count(void) { return s_count; }
PortWrite fake_port_write_at(size_t i)
{
    PortWrite none = { PORT_WRITE_DIRECTION, -1 };
    return (i < s_count) ? s_log[i] : none;
}
void fake_port_clear_writes(void) { s_count = 0; }

static void record(PortWriteKind k, int v)
{
    if (s_count < FAKE_PORT_MAX_WRITES) {
        s_log[s_count].kind = k;
        s_log[s_count].value = v;
        s_count++;
    }
}

bool rvc_port_read_left_sensor(void) { s_left_reads++; return s_left; }
bool rvc_port_read_right_sensor(void) { s_right_reads++; return s_right; }
bool rvc_port_read_dust_sensor(void) { s_dust_reads++; return s_dust; }
void rvc_port_wait_tick(void) { s_wait_calls++; }
void rvc_port_write_direction(MotorCommand direction) { record(PORT_WRITE_DIRECTION, (int)direction); }
void rvc_port_write_clean(CleanerCommand clean) { record(PORT_WRITE_CLEAN, (int)clean); }
