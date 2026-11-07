/* Test double for the Obstacle Location and Dust Existence modules. */
#ifndef FAKE_STORES_H
#define FAKE_STORES_H

#include "rvc_types.h"

#ifdef __cplusplus
extern "C" {
#endif

void fake_stores_reset(void);
void fake_stores_set(bool front, bool left, bool right, bool dust);
int fake_stores_obstacle_gets(void);
int fake_stores_dust_gets(void);

#ifdef __cplusplus
}
#endif
#endif
