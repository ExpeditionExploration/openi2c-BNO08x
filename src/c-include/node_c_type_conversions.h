
#ifndef NODE_C_TYPE_CONVERSIONS
#define NODE_C_TYPE_CONVERSIONS

#include <node/node_api.h>

#include "sh2/sh2.h"

// C->NAPI
napi_value c_to_NapiSensorEvent(napi_env env, sh2_SensorEvent_t *ev);
napi_value c_to_ProductId(napi_env env, sh2_ProductId_t *id);
napi_value c_to_SensorIdEnum(napi_env env);
napi_value c_to_SensorConfig(napi_env env, sh2_SensorConfig_t *cfg);
napi_value c_to_SensorMetadata(napi_env env, sh2_SensorMetadata_t *meta);
napi_value c_to_SensorEvent(napi_env env, sh2_SensorEvent_t *ev);
napi_value c_to_ErrorRecord(napi_env env, sh2_ErrorRecord_t *err);
napi_value c_to_Counts(napi_env env, sh2_Counts_t *counts);
napi_value c_to_AsyncEventId(napi_env env);
napi_value c_to_AsyncEvent(napi_env env, sh2_AsyncEvent_t *ev);
napi_value c_to_SensorConfigResp(napi_env env, sh2_SensorConfigResp_t *cfg);

// NAPI->C
int8_t from_SensorConfig_to_c(napi_env env, napi_value value,
                               sh2_SensorConfig_t *result);

#endif
