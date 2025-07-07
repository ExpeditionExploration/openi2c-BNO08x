#ifndef TEST_TYPE_CONVERSIONS_H
#define TEST_TYPE_CONVERSIONS_H

#include <node/node_api.h>

#include "sh2/sh2.h"

/**
 * Tests that SensorConfig JS object is properly converted to the C struct.
 *
 * Should throw a JavaScript error when something goes awry or test fails.
 */
napi_value test_node_to_c_SensorConfig(napi_env env, napi_callback_info info);

/**
 * Tests that SensorConfig C struct is properly converted to JavaScript
 * SensorConfig.
 */
napi_value test_node_from_c_SensorConfig(napi_env env, napi_callback_info info);

/**
 * Tests that SensorConfigResp C struct is properly converted to JavaScript
 * object.
 */
napi_value test_node_from_c_SensorConfigResp(napi_env env,
                                             napi_callback_info info);

/**
 * Tests that sh2_SensorEvent_t is proprly translated into a JavaScript object.
 */
napi_value test_node_from_c_SensorEvent(napi_env env, napi_callback_info info);

#endif
