
#include <node/node_api.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "c-tests/test_sensor_report_auxiliary.h"
#include "c-tests/test_type_conversions.h"
#include "error.h"

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

napi_value init(napi_env env, napi_value exports) {
    register_fn(env, exports, "test_node_to_c_SensorConfig",
                test_node_to_c_SensorConfig, NULL);
    register_fn(env, exports, "test_node_from_c_SensorConfig",
                test_node_from_c_SensorConfig, NULL);
    register_fn(env, exports, "test_node_from_c_SensorConfigResp",
                test_node_from_c_SensorConfigResp, NULL);
    register_fn(env, exports, "test_node_from_c_SensorEvent",
                test_node_from_c_SensorEvent, NULL);
    register_fn(env, exports, "test_add_properties_to_acceleration_report",
                test_add_properties_to_acceleration_report, NULL);
    register_fn(env, exports, "test_add_properties_to_rotation_vector",
                test_add_properties_to_rotation_vector, NULL);
    return exports;
}
NAPI_MODULE(bno08x_native, init)
