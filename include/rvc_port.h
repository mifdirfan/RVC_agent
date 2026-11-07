/**
 * @file rvc_port.h
 * @brief Environment ports (SC Section 5.5). Declared only; defined by the
 *        platform, by test doubles (TDD) or by the web simulator.
 *        The platform calls front_sensor_on_interrupt() (front_sensor.h).
 */
#ifndef RVC_PORT_H
#define RVC_PORT_H

#include "rvc_types.h"

#ifdef __cplusplus
extern "C" {
#endif

bool rvc_port_read_left_sensor(void);
bool rvc_port_read_right_sensor(void);
bool rvc_port_read_dust_sensor(void);
void rvc_port_wait_tick(void);
void rvc_port_write_direction(MotorCommand direction);
void rvc_port_write_clean(CleanerCommand clean);

#ifdef __cplusplus
}
#endif

#endif /* RVC_PORT_H */
