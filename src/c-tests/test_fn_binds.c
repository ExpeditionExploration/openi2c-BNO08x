
#include <node/node_api.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "error.h"
#include "node_c_type_conversions.h"
#include "sh2/sh2.h"

static uint8_t register_fn(napi_env env, napi_value exports, const char* name,
                           napi_callback cb, void* context) {
    napi_value fn;
    napi_status status;
    status = napi_create_function(env, name, strlen(name), cb, context, &fn);
    status |= napi_set_named_property(env, exports, name, fn);
    if (status != napi_ok) {
        napi_throw_error(env, "init c extension error",
                         "couldn't register an fn");
        return 1;
    }

    return 0;
}

/**
 * Tests that SensorConfig JS object is properly converted to the C struct.
 *
 * Should throw a JavaScript error when something goes awry or test fails.
 */
napi_value test_from_SensorConfig_to_c(napi_env env, napi_callback_info info) {
    size_t argc = 1;
    napi_value argv[1];

    // Get the provided js config
    napi_status status = napi_get_cb_info(env, info, &argc, argv, NULL, NULL);
    if (argc != 1) {
        napi_throw_error(env, ERROR_EXECUTING_TEST,
                         "Expected exactly 1 argument.");
        return NULL;
    }
    if (status != napi_ok) {
        napi_throw_error(env, ERROR_EXECUTING_TEST,
                         "The argument must be a SensorConfig object.");
        return NULL;
    }
    napi_value js_test_cfg = argv[0];

    sh2_SensorConfig_t test_cfg = {0};
    uint8_t code = from_SensorConfig_to_c(env, js_test_cfg, &test_cfg);
    if (code) {
        napi_throw_error(env, ERROR_EXECUTING_TEST,
                         "from_SensorConfig_to_c(..) returned error. Is the "
                         "config object good?");
        return NULL;
    }

    // Assertions
    if (test_cfg.batchInterval_us != 12345) {
        char s[200];
        snprintf(s, 200,
                 "sh2_SensorConfig_t.batchInterval_us was not properly "
                 "translated from JavaScript object. expected:12345!=got:%u",
                 test_cfg.batchInterval_us);
        napi_throw_error(env, ASSERT_ERROR, s);
        return NULL;
    }
    if (test_cfg.changeSensitivityEnabled != true) {
        napi_throw_error(
            env, ASSERT_ERROR,
            "sh2_SensorConfig_t.changeSensitivityEnabled was not properly "
            "translated from JavaScript object.");
        return NULL;
    }
    if (test_cfg.changeSensitivity != 12345) {
        napi_throw_error(
            env, ASSERT_ERROR,
            "sh2_SensorConfig_t.changeSensitivity was not properly "
            "translated from JavaScript object.");
        return NULL;
    }
    if (test_cfg.wakeupEnabled != true) {
        napi_throw_error(env, ASSERT_ERROR,
                         "sh2_SensorConfig_t.wakeupEnabled was not properly "
                         "translated from JavaScript object.");
        return NULL;
    }
    if (test_cfg.alwaysOnEnabled != true) {
        napi_throw_error(env, ASSERT_ERROR,
                         "sh2_SensorConfig_t.alwaysOnEnabled was not properly "
                         "translated from JavaScript object.");
        return NULL;
    }
    if (test_cfg.sniffEnabled != true) {
        napi_throw_error(env, ASSERT_ERROR,
                         "sh2_SensorConfig_t.sniffEnabled was not properly "
                         "translated from JavaScript object.");
        return NULL;
    }
    if (test_cfg.reportInterval_us != 12345) {
        napi_throw_error(
            env, ASSERT_ERROR,
            "sh2_SensorConfig_t.reportInterval_us was not properly "
            "translated from JavaScript object.");
        return NULL;
    }

    return NULL;
}

napi_value init(napi_env env, napi_value exports) {
    register_fn(env, exports, "test_from_SensorConfig_to_c",
                test_from_SensorConfig_to_c, NULL);
    return exports;
}
NAPI_MODULE(bno08x_native, init)
