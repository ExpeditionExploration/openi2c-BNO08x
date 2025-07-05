#include <node/node_api.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "error.h"
#include "node_c_type_conversions.h"

napi_value test_node_to_c_SensorConfig(napi_env env, napi_callback_info info) {
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
    uint8_t code = node_to_c_SensorConfig(env, js_test_cfg, &test_cfg);
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

napi_value test_node_from_c_SensorConfig(napi_env env,
                                         napi_callback_info info) {
    sh2_SensorConfig_t test_config = {
        .alwaysOnEnabled = true,
        .changeSensitivityEnabled = true,
        .changeSensitivityRelative = true,
        .sniffEnabled = true,
        .wakeupEnabled = true,
        .reportInterval_us = 12345,
        .batchInterval_us = 12345,
        .changeSensitivity = 12345,
    };
    napi_value result = node_from_c_SensorConfig(env, &test_config);
    return result; // Assert in TypeScript since it's way less wordy.
}

napi_value test_node_from_c_SensorConfigResp(napi_env env,
                                             napi_callback_info info) {
    sh2_SensorConfigResp_t test_cfg = {
        .sensorConfig =
            {
                .alwaysOnEnabled = true,
                .changeSensitivityEnabled = true,
                .changeSensitivityRelative = true,
                .sniffEnabled = true,
                .wakeupEnabled = true,
                .reportInterval_us = 12345,
                .batchInterval_us = 12345,
                .changeSensitivity = 12345,
            },
        .sensorId = SH2_ACCELEROMETER};
    napi_value result = node_from_c_SensorConfigResp(env, &test_cfg);
    return result; // Assert in TypeScript.
}
