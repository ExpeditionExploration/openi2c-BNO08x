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

napi_value test_node_from_c_SensorEvent(napi_env env, napi_callback_info info) {
    sh2_SensorEvent_t ev = {.delay_uS = 12345,
                            .timestamp_uS = 12345,
                            .len = 2,
                            .report = {123u, 123u},
                            .reportId = SH2_ACCELEROMETER};
    napi_value node_ev = node_from_c_SensorEvent(env, &ev);
    if (node_ev == NULL) {
        napi_throw_error(
            env, ERROR_TRANSLATING_STRUCT_TO_NODE,
            "node_from_c_SeonsorEvent(..) couldn't convert a sane event");
        return NULL;
    }
    return node_ev;
}

napi_value test_node_from_c_AsyncEvent(napi_env env, napi_callback_info info) {
    // AsyncEvent with SHTP event
    sh2_AsyncEvent_t test_s_with_shtp_event = {
        .eventId = SH2_SHTP_EVENT,
        .shtpEvent = SH2_SHTP_INTERRUPTED_PAYLOAD,
    };

    // AsyncEvent with feature resp
    sh2_SensorConfig_t c = {0};
    sh2_SensorConfigResp_t r = {
        .sensorId = SH2_ACCELEROMETER,
        .sensorConfig = c,
    };
    sh2_AsyncEvent_t test_s_with_sensor_config_resp = {
        .eventId = SH2_GET_FEATURE_RESP,
        .sh2SensorConfigResp = r,
    };

    // Out object and the two resulting test objects inside with keys
    // 'withShtpEv' and 'withSensorConfigResp'
    napi_value out, with_shtp_ev, with_sensor_config_resp;

    napi_status status = napi_create_object(env, &out);
    if (status != napi_ok) {
        napi_throw_error(env, ERROR_EXECUTING_TEST, "Couldn't create out obj");
        return NULL;
    }

    with_shtp_ev = node_from_c_AsyncEvent(env, &test_s_with_shtp_event);
    with_sensor_config_resp =
        node_from_c_AsyncEvent(env, &test_s_with_sensor_config_resp);

    if (with_shtp_ev == NULL || with_sensor_config_resp == NULL) {
        napi_throw_error(env, ERROR_EXECUTING_TEST,
                         "Couldn't convert value because of an error in test");
        return NULL;
    }

    status = napi_set_named_property(env, out, "withShtpEv", with_shtp_ev);
    if (status != napi_ok) {
        napi_throw_error(env, ERROR_EXECUTING_TEST,
                         "Couldn't set prop for out var");
        return NULL;
    }
    status = napi_set_named_property(env, out, "withSensorConfigResp",
                                     with_sensor_config_resp);
    if (status != napi_ok) {
        napi_throw_error(env, ERROR_EXECUTING_TEST,
                         "Couldn't set prop for out var");
        return NULL;
    }

    return out;
}
