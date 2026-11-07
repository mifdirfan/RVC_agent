/* Test double for Motor Interface and Cleaner Interface: records every send call. */
#ifndef FAKE_INTERFACES_H
#define FAKE_INTERFACES_H

#include <stddef.h>
#include "rvc_types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum { IF_MOTOR, IF_CLEANER } InterfaceKind;
typedef struct { InterfaceKind kind; int value; } InterfaceCall;

void fake_interfaces_reset(void);
size_t fake_interfaces_count(void);
InterfaceCall fake_interfaces_at(size_t i);

#ifdef __cplusplus
}
#endif
#endif
