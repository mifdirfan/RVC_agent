/**
 * @file dust_existence.h
 * @brief Dust Existence: Process 1.6 and its data store (information hiding).
 */
#ifndef DUST_EXISTENCE_H
#define DUST_EXISTENCE_H

#include "rvc_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Sets the store to false (C21). */
void dust_existence_init(void);
/** Stores the Dust value of this Tick. */
void dust_existence_set(bool dust);
/** Returns the stored Dust Existence. */
bool dust_existence_get(void);

#ifdef __cplusplus
}
#endif

#endif /* DUST_EXISTENCE_H */
