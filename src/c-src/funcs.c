#include <node/node_api.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "error.h"
#include "node_c_type_conversions.h"
#include "sh2/sh2.h"
#include "sh2/sh2_hal.h"
#include "sh2_hal_supplement.h"

// Max arguments a function can take
#define MAX_ARGUMENTS 10

/**
 * Auxialiry func to parse napi_callback arguments from Node
 *
 * @return success
 */
bool parse_args(napi_env env, napi_callback_info info, size_t *argc,
                napi_value *argv, napi_value *this, void **data,
                size_t argc_min, size_t argc_max) {
    napi_status status = napi_get_cb_info(env, info, argc, argv, this, data);
    switch (status) {
        case napi_ok: break;

        case napi_invalid_arg:
            napi_throw_error(env, ARGUMENT_ERROR, "Invalid argument");
            return false;

        case napi_object_expected:
            napi_throw_error(env, ARGUMENT_ERROR, "Object expected");
            return false;

        case napi_string_expected:
            napi_throw_error(env, ARGUMENT_ERROR, "String expected");
            return false;

        case napi_name_expected:
            napi_throw_error(env, ARGUMENT_ERROR, "Name expected");
            return false;

        case napi_function_expected:
            napi_throw_error(env, ARGUMENT_ERROR, "Function expected");
            return false;

        case napi_number_expected:
            napi_throw_error(env, ARGUMENT_ERROR, "Number expected");
            return false;

        case napi_boolean_expected:
            napi_throw_error(env, ARGUMENT_ERROR, "Boolean expected");
            return false;

        case napi_array_expected:
            napi_throw_error(env, ARGUMENT_ERROR, "Array expected");
            return false;

        case napi_generic_failure:
            napi_throw_error(env, ARGUMENT_ERROR, "Generic failure");
            return false;

        case napi_pending_exception:
            napi_throw_error(env, ARGUMENT_ERROR, "Pending exeption");
            return false;

        case napi_cancelled:
            napi_throw_error(env, ARGUMENT_ERROR, "Cancelled");
            return false;

        case napi_escape_called_twice:
            napi_throw_error(env, ARGUMENT_ERROR, "Escape called twice");
            return false;

        case napi_handle_scope_mismatch:
            napi_throw_error(env, ARGUMENT_ERROR, "Scope mismatch");
            return false;

        case napi_callback_scope_mismatch:
            napi_throw_error(env, ARGUMENT_ERROR, "Callback scope mismatch");
            return false;

        case napi_queue_full:
            napi_throw_error(env, ARGUMENT_ERROR, "Napi queue full");
            return false;

        case napi_closing:
            napi_throw_error(env, ARGUMENT_ERROR, "Closing");
            return false;

        case napi_bigint_expected:
            napi_throw_error(env, ARGUMENT_ERROR, "Bigint expected");
            return false;

        case napi_date_expected:
            napi_throw_error(env, ARGUMENT_ERROR, "Date expected");
            return false;

        case napi_arraybuffer_expected:
            napi_throw_error(env, ARGUMENT_ERROR, "Arraybuffer expected");
            return false;

        case napi_detachable_arraybuffer_expected:
            napi_throw_error(env, ARGUMENT_ERROR,
                             "Detatchable arraybuffer expected");
            return false;

        case napi_would_deadlock: /* unused */
        case napi_no_external_buffers_allowed:
            napi_throw_error(env, ARGUMENT_ERROR,
                             "No external buffers allowed");
            return false;

        case napi_cannot_run_js:
            napi_throw_error(env, ARGUMENT_ERROR, "=D");
            return false;

        default:
            napi_throw_error(env, UNKNOWN_ERROR,
                             "A new, unexpected error happened.");
            return false;
    }

    if (*argc < argc_min || *argc > argc_max) {
        napi_throw_error(env, ARGUMENT_ERROR, "Too many or too few arguments");
        return false;
    }
    return true;
}

// This function creates struct with settings to communicated with a SB2 device.
//
// args:
//  - bus_number: uint8_t
//  - addr: uint8_t
napi_value cb_setI2CSettings(napi_env env, napi_callback_info info) {
    napi_value argv[MAX_ARGUMENTS] = {NULL};
    napi_value this;
    size_t argc = MAX_ARGUMENTS;
    void *data = NULL;

    bool success = parse_args(env, info, &argc, argv, &this, &data, 2, 2);
    if (!success) { return NULL; }

    napi_status status;
    uint32_t bus, addr;

    status = napi_get_value_uint32(env, argv[0], &bus);
    status |= napi_get_value_uint32(env, argv[1], &addr);

    if (status != napi_ok || bus > 0xFF || addr > 0xFF) {
        napi_throw_error(env, ARGUMENT_ERROR,
                         "Probably argument out of range. Settings not set. "
                         "fn: cb_setI2CSettings");
        return NULL;
    }

    sh2_Hal_t hal = make_hal();
    i2c_settings_t i2c_settings = {.bus = bus, .addr = addr, .hal = hal};
    set_i2c_settings(&i2c_settings);

    return NULL;
}

// This function gets current I2C settings and returns them.
//
// If no settings have been saved, the contents should be zeroes.
//
// args: None
napi_value cb_getI2CSettings(napi_env env, napi_callback_info _) {
    napi_status status;

    i2c_settings_t settings = get_i2c_settings();

    napi_value bus, addr, obj;
    status = napi_create_uint32(env, settings.bus, &bus);
    status |= napi_create_uint32(env, settings.addr, &addr);
    status |= napi_create_object(env, &obj);
    status |= napi_set_named_property(env, obj, "bus", bus);
    status |= napi_set_named_property(env, obj, "addr", addr);
    if (status != napi_ok) {
        napi_throw_error(env, ERROR_TRANSLATING_STRUCT_TO_NODE,
                         "Couldn't construct I2C settings");
        return NULL;
    }
    return obj;
}

napi_value cb_service(napi_env env, napi_callback_info info) {
    // i2c_settings_t settings = get_i2c_settings();
    sh2_service();
    return NULL;
}

// This struct is used to pass it as a cookie for a sh2_SensorCallback_t.
// It's signature is void (void *, sh2_SensorEvent_t *)
//
// Also cb_sh2_open uses this for cookies.
typedef struct {
    napi_env env;
    napi_value jsFn;
    napi_value cookie;
    napi_ref jsFn_ref;
    napi_ref cookie_ref;
} cb_cookie_t;

// This function is the common C callback the driver calls on a sensor event.
// It then calls the JS callback that is passed in the cookie structure
// `cb_cookie_t`.
static void sensor_callback(void *cookie, sh2_SensorEvent_t *event) {
    napi_env env = (napi_env)cookie;
    napi_status status;

    // Translate sensor event to napi_value
    napi_value sensor_event = c_to_SensorEvent(env, event);
    if (sensor_event == NULL) { return; }

    // Cast the cookie and prepare the arguments
    // to call the JS function
    cb_cookie_t *c = (cb_cookie_t *)cookie;
    napi_value argv[2] = {c->cookie, sensor_event};

    // Call the callback function with the sensor data
    napi_value this;
    status = napi_get_global(c->env, &this);
    status |= napi_call_function(c->env, this, c->jsFn, 2, argv, NULL);
    if (status != napi_ok) {
        napi_throw_error(
            c->env, ERROR_CREATING_NAPI_VALUE,
            "Sensor event happened but couldn't construct a napi value for it");
        return;
    }
}

static cb_cookie_t *_sensor_callback;
// This function prepares the `cb_cookie_t` struct and calls the
// sh2_setSensorCallback function.
// It sets the callback function to be called when a sensor event occurs by
// passing the `cb_cookie_t` struct in the cookie parameter.
//
// args:
//  - jsFn: function to be called when a sensor event occurs
//  - cookie: a value that will be passed to the sensor callback function
napi_value cb_setSensorCallback(napi_env env, napi_callback_info info) {
    // napi_status status;

    napi_value argv[MAX_ARGUMENTS] = {NULL};
    size_t argc = MAX_ARGUMENTS;

    bool success = parse_args(env, info, &argc, argv, NULL, NULL, 2, 2);
    if (!success) {
        napi_throw_error(env, ARGUMENT_ERROR,
                         "Couldn't parse arguments in cb_setSensorCallback");
        return NULL;
    }

    // Prepare the cookie struct
    cb_cookie_t *cookie = malloc(sizeof(cb_cookie_t));
    if (cookie == NULL) {
        napi_throw_error(env, ERROR_CREATING_NAPI_VALUE,
                         "Couldn't allocate memory for cookie to use for "
                         "sensor callback");
        return NULL;
    }

    // Delete ref to allow GC to take it, and make note of the new ref
    if (_sensor_callback != NULL) {
        napi_delete_reference(env, _sensor_callback->jsFn_ref);
        napi_delete_reference(env, _sensor_callback->cookie_ref);
        free(_sensor_callback);
        _sensor_callback = NULL;
    }

    cookie->env = env;
    cookie->jsFn = argv[0];
    cookie->cookie = argv[1];
    napi_create_reference(env, cookie->jsFn, 1, &cookie->jsFn_ref);
    napi_create_reference(env, cookie->cookie, 1, &cookie->cookie_ref);
    _sensor_callback = cookie;

    int8_t ret_code;
    if ((ret_code = sh2_setSensorCallback(sensor_callback, _sensor_callback)) <
        0) {
        printf("Setting a new callback failed with code: %hhd\n", ret_code);
    }

    return NULL;
}

static void async_event_callback_broker(void *cookie, sh2_AsyncEvent_t *event) {
    cb_cookie_t *cookie_with_type = cookie;
    napi_status status;
    napi_value return_value;

    // Translate sensor event to napi_value
    printf(
        "env: %p, async_event_callback_broker(void *cookie, "
        "sh2_AsyncEvent_t*)\n",
        (void *)cookie_with_type->env);
    napi_value async_event = c_to_AsyncEvent(cookie_with_type->env, event);
    if (async_event == NULL) { return; }

    // Cast the cookie and prepare the arguments
    // to call the JS function
    napi_value argv[2] = {cookie_with_type->cookie, async_event};

    // Call the callback function with the sensor data
    napi_value this;
    status = napi_get_global(cookie_with_type->env, &this);
    status |=
        napi_call_function(cookie_with_type->env, this, cookie_with_type->jsFn,
                           2, argv, &return_value);
    if (status != napi_ok) {
        napi_throw_error(
            cookie_with_type->env, ERROR_CREATING_NAPI_VALUE,
            "Sensor event happened but couldn't construct a napi value for it");
        return;
    }
}

napi_value cb_sh2_open(napi_env env, napi_callback_info info) {
    napi_status status;
    size_t argc = 2;
    napi_value argv[2] = {NULL};
    napi_value this;

    status = napi_get_global(env, &this);
    if (status != napi_ok) {
        napi_throw_error(env, ARGUMENT_ERROR,
                         "Couldn't get value this from napi");
        return NULL;
    }
    bool okkay = parse_args(env, info, &argc, argv, &this, NULL, 2, 2);
    if (okkay == false) {
        napi_throw_error(env, ARGUMENT_ERROR,
                         "Couldn't parse arguments in cb_sh2_open");
        return NULL;
    }

    // Get napi values of the arguments
    napi_value jsCookie = argv[1];
    napi_value jsFn = argv[0];

    // Get the bus number and address from the I2C settings
    // i2c_settings_t     settings = get_i2c_settings();
    if (_sensor_callback != NULL) {
        napi_delete_reference(env, _sensor_callback->jsFn_ref);
        napi_delete_reference(env, _sensor_callback->cookie_ref);
        free(_sensor_callback);
        _sensor_callback = NULL;
    }
    cb_cookie_t *cookie = malloc(sizeof(cb_cookie_t));
    _sensor_callback = cookie;
    _sensor_callback->env = env;
    _sensor_callback->jsFn = jsFn;
    _sensor_callback->cookie = jsCookie;

    // Prevents jsFn and cookie from being garbage collected in case
    // there are no references left in the node side of things.
    napi_create_reference(env, _sensor_callback->jsFn, 1,
                          &_sensor_callback->jsFn_ref);
    napi_create_reference(env, _sensor_callback->cookie, 1,
                          &_sensor_callback->cookie_ref);

    // Prepare the HAL struct
    static sh2_Hal_t hal;
    hal = make_hal();

    // Open connection
    printf("env: %p, cb_sh2_open(napi_env env, napi_callback_info info)\n",
           (void *)env);
    int status_ = sh2_open(&hal, async_event_callback_broker, _sensor_callback);
    if (status_ != 0) {
        napi_throw_error(env, ERROR_INTERACTING_WITH_DRIVER,
                         "Couldn't open the sh2 device");
    }

    return NULL;
}

napi_value cb_sh2_close(napi_env env, napi_callback_info _) {
    sh2_close();
    return NULL;
}
