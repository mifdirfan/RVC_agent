/**
 * @file dbg_move_backward.c
 * @brief Compiles the unmodified src/move_backward.c and exposes its active
 *        flag read-only. Linked INSTEAD of src/move_backward.c.
 */
#include "move_backward.c"
#include "rvc_debug.h"

bool rvc_dbg_move_backward_active(void)
{
    return s_active;
}
