#ifndef _SENSOR_REPORT_AUXIALIRY_FNS_H
#define _SENSOR_REPORT_AUXIALIRY_FNS_H

#include <node/node_api.h>
#include <stdint.h>

/// Add .getX(), .getY() and .getZ() methods to the report object. Non-zero
/// return value indicates an error.
uint8_t add_getters_to_acceleration_report(napi_env env, napi_value report);

#endif
