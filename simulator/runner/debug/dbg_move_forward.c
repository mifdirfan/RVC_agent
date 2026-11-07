/**
 * @file dbg_move_forward.c
 * @brief Compiles the unmodified src/move_forward.c and exposes its active flag
 *        read-only. Linked INSTEAD of src/move_forward.c.
 */
#include "move_forward.c"
#include "rvc_debug.h"

bool rvc_dbg_move_forward_active(void)
{
    return s_active;
}
