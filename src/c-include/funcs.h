#ifndef FUNCS_H
#define FUNCS_H

#include <node/node_api.h>

napi_value cb_setI2CSettings(napi_env env, napi_callback_info info);
napi_value cb_getI2CSettings(napi_env env, napi_callback_info _);
napi_value cb_sh2_open(napi_env env, napi_callback_info info);
napi_value cb_sh2_close(napi_env env, napi_callback_info info);
napi_value cb_service(napi_env env, napi_callback_info info);
napi_value cb_setSensorCallback(napi_env env, napi_callback_info info);
napi_value cb_get_sensor_config(napi_env env, napi_callback_info info);
napi_value cb_set_sensor_config(napi_env env, napi_callback_info info);

#endif
