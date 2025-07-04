#ifndef TEST_TYPE_CONVERSIONS_H
#define TEST_TYPE_CONVERSIONS_H

#include <node/node_api.h>

#include "sh2/sh2.h"

/**
 * Tests that SensorConfig JS object is properly converted to the C struct.
 *
 * Should throw a JavaScript error when something goes awry or test fails.
 */
napi_value test_from_SensorConfig_to_c(napi_env env, napi_callback_info info);

/**
 * Tests that SensorConfig C struct is properly converted to JavaScript
 * SensorConfig.
 */
napi_value test_c_to_SensorConfig(napi_env env, napi_callback_info info);

#endif
