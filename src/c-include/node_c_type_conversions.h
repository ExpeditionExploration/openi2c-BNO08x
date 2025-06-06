
#ifndef NODE_C_TYPE_CONVERSIONS
#define NODE_C_TYPE_CONVERSIONS

#include <node/node_api.h>

#include "sh2/sh2.h"

// C->NAPI
napi_value mkNapiSensorEvent(napi_env env, sh2_SensorEvent_t *ev);
napi_value mkProductId(napi_env env, sh2_ProductId_t *id);
napi_value mkSensorIdEnum(napi_env env);
napi_value mkSensorConfig(napi_env env, sh2_SensorConfig_t *cfg);
napi_value mkSensorMetadata(napi_env env, sh2_SensorMetadata_t *meta);
napi_value mkSensorEvent(napi_env env, sh2_SensorEvent_t *ev);
napi_value mkErrorRecord(napi_env env, sh2_ErrorRecord_t *err);
napi_value mkCounts(napi_env env, sh2_Counts_t *counts);
napi_value mkTareBasis(napi_env env);
napi_value mkTareAxis(napi_env env);
napi_value mkQuaternion(napi_env env, sh2_Quaternion_t *qt);
napi_value mkMotionIntent(napi_env env);
napi_value mkMotionRequest(napi_env env);
napi_value mkAsyncEventId(napi_env env);
napi_value mkAsyncEvent(napi_env env, sh2_AsyncEvent_t *ev);
napi_value mkSensorConfigResp(napi_env env, sh2_SensorConfigResp_t *cfg);
napi_value mkShtpEvent(napi_env env);

// NAPI->C
uint8_t parseSensorConfig(napi_env env, napi_value value,
                          sh2_SensorConfig_t *result);

#endif
