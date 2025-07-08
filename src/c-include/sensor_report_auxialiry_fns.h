#ifndef _SENSOR_REPORT_AUXIALIRY_FNS_H
#define _SENSOR_REPORT_AUXIALIRY_FNS_H

#include <node/node_api.h>
#include <stdint.h>

/// Add .x, .y and .z properties to the report object. Non-zero
/// return value indicates an error.
uint8_t add_xyz_to_sensor_report(napi_env env, napi_value report);

/// Add .yaw, .pitch, .roll properties for getting euler angles. Also add
/// .i, .j, .k, .real properties for accessing the same as quaternion.
uint8_t add_pitch_yaw_roll_to_rotation_vector(napi_env env, napi_value report);

#endif
