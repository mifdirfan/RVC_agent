/**
 * @file dust_existence.c
 * @brief Dust Existence (Process 1.6 + data store). The store is hidden.
 */
#include "dust_existence.h"

static bool s_store;

void dust_existence_init(void)
{
    s_store = false; /* C21 */
}

void dust_existence_set(bool dust)
{
    s_store = dust;
}

bool dust_existence_get(void)
{
    return s_store;
}
