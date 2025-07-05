
#ifndef NODE_C_TYPE_CONVERSIONS
#define NODE_C_TYPE_CONVERSIONS

#include <node/node_api.h>

#include "sh2/sh2.h"

// C->NAPI
napi_value node_from_c_SensorConfig(napi_env env, sh2_SensorConfig_t *cfg);
napi_value node_from_c_SensorEvent(napi_env env, sh2_SensorEvent_t *ev);
napi_value node_from_c_AsyncEvent(napi_env env, sh2_AsyncEvent_t *ev);
napi_value node_from_c_SensorConfigResp(napi_env env,
                                        sh2_SensorConfigResp_t *cfg);

// NAPI->C
int8_t node_to_c_SensorConfig(napi_env env, napi_value value,
                              sh2_SensorConfig_t *result);

#endif
