/* Test double for the 7 action modules: records every control signal in order. */
#ifndef FAKE_ACTIONS_H
#define FAKE_ACTIONS_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    SIG_MF_ENABLE,  /* Enable  "Move Forward"   */
    SIG_MF_DISABLE, /* Disable "Move Forward"   */
    SIG_MB_ENABLE,  /* Enable  "Move Backward"  */
    SIG_MB_DISABLE, /* Disable "Move Backward"  */
    SIG_TL,         /* Trigger "Turn Left"      */
    SIG_TR,         /* Trigger "Turn Right"     */
    SIG_CN,         /* Trigger "Clean Normal"   */
    SIG_CO,         /* Trigger "Clean Off"      */
    SIG_PU          /* Trigger "Power-Up Clean" */
} Signal;

void fake_actions_reset(void);
size_t fake_actions_count(void);
Signal fake_actions_at(size_t i);

#ifdef __cplusplus
}
#endif
#endif
