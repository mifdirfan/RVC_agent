/* Test double for the six rvc_port_* functions (TDD rule 8). */
#ifndef FAKE_PORT_H
#define FAKE_PORT_H

#include <stddef.h>
#include "rvc_types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum { PORT_WRITE_DIRECTION, PORT_WRITE_CLEAN } PortWriteKind;

typedef struct {
    PortWriteKind kind;
    int value; /* MotorCommand or CleanerCommand */
} PortWrite;

#define FAKE_PORT_MAX_WRITES 1024

void fake_port_reset(void);
void fake_port_set_left(bool v);
void fake_port_set_right(bool v);
void fake_port_set_dust(bool v);
int fake_port_left_reads(void);
int fake_port_right_reads(void);
int fake_port_dust_reads(void);
int fake_port_wait_calls(void);
size_t fake_port_write_count(void);
PortWrite fake_port_write_at(size_t i);
void fake_port_clear_writes(void);

#ifdef __cplusplus
}
#endif
#endif
