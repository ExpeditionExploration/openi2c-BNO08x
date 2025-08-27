#include <node/node_api.h>
#include <stdint.h>
#include <string.h>

#include "funcs.h"

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
    register_fn(env, exports, "setI2CConfig", cb_setI2CSettings, NULL);
    register_fn(env, exports, "service", cb_service, NULL);
    register_fn(env, exports, "setSensorCallback", cb_setSensorCallback, NULL);
    register_fn(env, exports, "getSensorConfig", cb_get_sensor_config, NULL);
    register_fn(env, exports, "setSensorConfig", cb_set_sensor_config, NULL);
    register_fn(env, exports, "open", cb_sh2_open, NULL);
    register_fn(env, exports, "devOn", cb_devOn, NULL);
    register_fn(env, exports, "devReset", cb_devReset, NULL);
    register_fn(env, exports, "devSleep", cb_devSleep, NULL);
    register_fn(env, exports, "close", cb_sh2_close, NULL);
    register_fn(env, exports, "setFrs", cb_setFrs, NULL);
    register_fn(env, exports, "getFrs", cb_getFrs, NULL);
    // Expose interrupts setup
    register_fn(env, exports, "useInterrupts", cb_use_interrupts, NULL);
    return exports;
}
NAPI_MODULE(bno08x_native, init)
