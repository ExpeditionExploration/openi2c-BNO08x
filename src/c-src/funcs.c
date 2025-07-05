#include <node/node_api.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <uv.h>

#include "error.h"
#include "node_c_type_conversions.h"
#include "sh2/sh2.h"
#include "sh2/sh2_err.h"
#include "sh2/sh2_hal.h"
#include "sh2_hal_supplement.h"
#include "uv/unix.h"

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

// This struct is used to pass it as a cookie for a sh2_SensorCallback_t.
// It's signature is void (void *, sh2_SensorEvent_t *)
//
// Also cb_sh2_open uses this for cookies.
typedef struct {
    napi_env env;
    napi_ref jsFn_ref;
    napi_ref cookie_ref;
    uv_thread_t thread;
} cb_cookie_t;

static cb_cookie_t *_sensor_callback;
static cb_cookie_t *_async_event_callback;

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

napi_value cb_service(napi_env env, napi_callback_info info) {
    sh2_service();
    return NULL;
}

// This function is the common C callback the driver calls on a sensor event.
// It then calls the JS callback that is passed in the cookie structure
// `cb_cookie_t`.
static void sensor_callback(void *cookie, sh2_SensorEvent_t *event) {
    napi_env env = ((cb_cookie_t *)(cookie))->env;
    napi_status status;

    uv_thread_t this_thread = uv_thread_self();
    if (!uv_thread_equal(&this_thread, &((cb_cookie_t *)cookie)->thread)) {
        char *msg = "Not in NodeJS main thread. Can't invoke sensor callback.";
        napi_throw_error(env, THREADING_ERROR, msg);
        return;
    }

    // Translate sensor event to napi_value
    napi_value sensor_event = c_to_SensorEvent(env, event);
    if (sensor_event == NULL) {
        napi_throw_error(env, ERROR_TRANSLATING_STRUCT_TO_NODE,
                         "Error translating SensorEvent to JS object.");
        return;
    }

    // Cast the cookie and prepare the arguments
    // to call the JS function
    cb_cookie_t *c = (cb_cookie_t *)cookie;
    napi_value fetched_js_cookie;
    napi_value fetched_js_fn;
    status = napi_get_reference_value(env, c->cookie_ref, &fetched_js_cookie);
    if (status != napi_ok) {
        napi_throw_error(
            env, REF_ERROR,
            "Unable to fetch value by reference in sensor_callback.");
        return;
    }
    status = napi_get_reference_value(env, c->jsFn_ref, &fetched_js_fn);
    if (status != napi_ok) {
        napi_throw_error(
            env, REF_ERROR,
            "Unable to fetch value by reference in sensor_callback.");
        return;
    }
    napi_value argv[2] = {sensor_event, fetched_js_cookie};

    // Call the callback function with the sensor data
    napi_value global;
    status = napi_get_global(c->env, &global);
    if (status != napi_ok) {
        napi_throw_error(env, ERROR_CREATING_NAPI_VALUE,
                         "Couldn't get JS global object in sensor_callback");
        return;
    }
    napi_value return_value;
    status = napi_call_function(c->env, global, fetched_js_fn, 2, argv,
                                &return_value);
    if (status != napi_ok) {
        napi_throw_error(env, ERROR_CALLING_CB,
                         "Error calling SensorEvent callback.");
        return;
    }
    if (status != napi_ok) {
        napi_throw_error(
            c->env, ERROR_CREATING_NAPI_VALUE,
            "Sensor event happened but couldn't construct a napi value for it");
        return;
    }
}

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

    napi_value argv[2] = {0};
    size_t argc = 2;

    napi_valuetype argt;
    bool success = parse_args(env, info, &argc, argv, NULL, NULL, 2, 2);
    if (!success) {
        napi_throw_error(env, ARGUMENT_ERROR,
                         "Couldn't parse arguments in cb_setSensorCallback");
        return NULL;
    }
    napi_typeof(env, argv[0], &argt);
    if (argt != napi_function) {
        napi_throw_error(env, ARGUMENT_ERROR,
                         "First argument must be a function.");
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
    cookie->thread = uv_thread_self();
    napi_status status =
        napi_create_reference(env, argv[0], 1, &cookie->jsFn_ref);
    if (status != napi_ok) {
        napi_throw_error(env, REF_ERROR,
                         "Couldn't create a napi ref for callback value in "
                         "setSensorCallback.");
    }
    status = napi_create_reference(env, argv[1], 1, &cookie->cookie_ref);
    if (status != napi_ok) {
        napi_throw_error(env, REF_ERROR,
                         "Couldn't create a napi ref for cookie value in "
                         "setSensorCallback.");
    }
    _sensor_callback = cookie;

    int8_t code = sh2_setSensorCallback(sensor_callback, _sensor_callback);
    if (code != SH2_OK) {
        char msg[200];
        snprintf(msg, 200, "Setting a new callback failed with code: %hhd\n",
                 code);
        napi_throw_error(env, ERROR_INTERACTING_WITH_DRIVER, msg);
        return NULL;
    }

    return NULL;
}

static void async_event_callback_broker(void *cookie, sh2_AsyncEvent_t *event) {
    cb_cookie_t *cookie_with_type = cookie;
    napi_status status;
    napi_value return_value;
    uv_thread_t this_thread = uv_thread_self();
    if (!uv_thread_equal(&this_thread, &cookie_with_type->thread)) {
        char *msg = "Not in NodeJS main thread.";
        napi_throw_error(cookie_with_type->env, THREADING_ERROR, msg);
        return;
    }

    // Translate sensor event to napi_value
    napi_value async_event = c_to_AsyncEvent(cookie_with_type->env, event);
    if (async_event == NULL) {
        napi_throw_error(cookie_with_type->env, ERROR_CREATING_NAPI_VALUE,
                         "Error converting sh2_AsyncEvent_t to JS object.");
        return;
    }

    // Since this is a callback, and the extension exits to node entirely
    // between cb_service()-calls, the handles need to be fetched again by
    // reference.
    napi_handle_scope scope;
    status = napi_open_handle_scope(cookie_with_type->env, &scope);
    if (status != napi_ok) {
        napi_throw_error(cookie_with_type->env, ERROR_OPENING_SCOPE,
                         "Couldn't open napi scope.");
        return;
    }
    napi_value cookie_cookie_arg;
    napi_value cookie_jsFn_arg;
    status = napi_get_reference_value(cookie_with_type->env,
                                      cookie_with_type->cookie_ref,
                                      &cookie_cookie_arg);
    if (status != napi_ok) {
        napi_throw_error(
            cookie_with_type->env, REF_ERROR,
            "Error getting value by reference in async_event_callback_broker");
        return;
    }
    status = napi_get_reference_value(
        cookie_with_type->env, cookie_with_type->jsFn_ref, &cookie_jsFn_arg);
    if (status != napi_ok) {
        napi_throw_error(
            cookie_with_type->env, REF_ERROR,
            "Error getting value by reference in async_event_callback_broker");
        return;
    }

    // Cast the cookie and prepare the arguments
    // to call the JS function
    napi_value argv[2] = {async_event, cookie_cookie_arg};

    // Call the callback function with the sensor data
    napi_value global;
    status = napi_get_global(cookie_with_type->env, &global);
    if (status != napi_ok) {
        napi_throw_error(cookie_with_type->env, ERROR_CREATING_NAPI_VALUE,
                         "Couldn't get global object.");
        return;
    }

    status = napi_call_function(cookie_with_type->env, global, cookie_jsFn_arg,
                                2, argv, &return_value);
    if (status != napi_ok) {
        napi_throw_error(
            cookie_with_type->env, ERROR_CALLING_CB,
            "Error calling async event (registered with open(..)) callback.");
        return;
    }
    // Close the scope
    napi_close_handle_scope(cookie_with_type->env, scope);
}

napi_value cb_sh2_open(napi_env env, napi_callback_info info) {
    napi_status status;
    size_t argc = 2;
    napi_value argv[2] = {0};
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
    napi_value jsFn = argv[0];
    napi_value jsCookie = argv[1];

    // Get the bus number and address from the I2C settings
    // i2c_settings_t     settings = get_i2c_settings();
    if (_async_event_callback != NULL) {
        napi_delete_reference(env, _async_event_callback->jsFn_ref);
        napi_delete_reference(env, _async_event_callback->cookie_ref);
        free(_async_event_callback);
        _async_event_callback = NULL;
    }
    cb_cookie_t *cookie = malloc(sizeof(cb_cookie_t));
    _async_event_callback = cookie;
    _async_event_callback->env = env;
    _async_event_callback->thread = uv_thread_self();

    // Prevents jsFn and cookie from being garbage collected in case
    // there are no references left in the node side of things.
    status =
        napi_create_reference(env, jsFn, 1, &_async_event_callback->jsFn_ref);
    if (status != napi_ok) {
        napi_throw_error(env, REF_ERROR,
                         "Couldn't create reference. Was JavaScript function "
                         "provided as a callback?\n");
        return NULL;
    }
    status = napi_create_reference(env, jsCookie, 1,
                                   &_async_event_callback->cookie_ref);
    if (status != napi_ok) {
        napi_throw_error(env, REF_ERROR,
                         "Couldn't create reference. Was JavaScript Object "
                         "provided as a cookie?\n");
        return NULL;
    }

    // Prepare the HAL struct
    static sh2_Hal_t hal;
    hal = make_hal();

    // Open connection
    int status_ =
        sh2_open(&hal, async_event_callback_broker, _async_event_callback);
    if (status_ != SH2_OK) {
        napi_throw_error(env, ERROR_INTERACTING_WITH_DRIVER,
                         "Couldn't open the sh2 device.");
    }

    return NULL;
}

napi_value cb_sh2_close(napi_env env, napi_callback_info _) {
    sh2_close();
    return NULL;
}

napi_value cb_get_sensor_config(napi_env env, napi_callback_info info) {
    size_t argc = 1;
    napi_value argv[1] = {0};

    bool okkay = parse_args(env, info, &argc, argv, NULL, NULL, 1, 1);
    if (okkay == false) {
        napi_throw_error(env, ARGUMENT_ERROR,
                         "Couldn't parse arguments in cb_sh2_open");
        return NULL;
    }

    // Get sensor id
    uint32_t sensor_id;
    napi_status status = napi_get_value_uint32(env, argv[0], &sensor_id);
    if (status != napi_ok) {
        fprintf(stderr, "status=%d\n", status);
        napi_throw_error(env, NULL, "Invalid SensorId");
    }

    // Get sensor config
    sh2_SensorConfig_t config;
    int code;
    if ((code = sh2_getSensorConfig(sensor_id, &config)) < 0) {
        printf("Failed to get sensor config for %u with code: %d\n", sensor_id,
               code);
        napi_throw_error(env, ERROR_INTERACTING_WITH_DRIVER,
                         "Failed to get sensor config");
        return NULL;
    }

    // Convert sensor config to napi_value
    napi_value result = c_to_SensorConfig(env, &config);
    if (result == NULL) {
        napi_throw_error(env, ERROR_CREATING_NAPI_VALUE,
                         "Failed to convert sensor config to napi_value");
        return NULL;
    }

    // Return the sensor config
    return result;
}

napi_value cb_set_sensor_config(napi_env env, napi_callback_info info) {
    size_t argc = 2;
    napi_value argv[2] = {0};

    bool okkay = parse_args(env, info, &argc, argv, NULL, NULL, 2, 2);
    if (okkay == false) {
        napi_throw_error(env, ARGUMENT_ERROR,
                         "Couldn't parse arguments in cb_set_sensor_config");
        return NULL;
    }

    // Convert sensor config to C struct
    // The first argument in argv is the sensor id, the second is the sensor
    // config
    static sh2_SensorConfig_t configs[SH2_MAX_SENSOR_ID + 1];
    sh2_SensorConfig_t config;
    if (from_SensorConfig_to_c(env, argv[1], &config) != 0) {
        napi_throw_error(env, ERROR_CREATING_NAPI_VALUE,
                         "Failed to convert sensor config from napi_value");
        return NULL;
    }

    // Read the sensor id from the first argument
    uint32_t sensor_id;
    napi_status status = napi_get_value_uint32(env, argv[0], &sensor_id);
    if (status != napi_ok) {
        napi_throw_error(env, ARGUMENT_ERROR,
                         "Failed to get sensor id from napi_value");
        return NULL;
    }

    // Move config to static memory
    memcpy(&configs[sensor_id], &config, sizeof(sh2_SensorConfig_t));

    // Set sensor config
    int code;
    if ((code = sh2_setSensorConfig(sensor_id, &configs[sensor_id])) !=
        SH2_OK) {
        printf("Failed to set sensor config with code: %d\n", code);
        napi_throw_error(env, ERROR_INTERACTING_WITH_DRIVER,
                         "Failed to set sensor config");
        return NULL;
    }

    return NULL;
}

napi_value cb_devOn(napi_env env, napi_callback_info info) {
    int code;
    if ((code = sh2_devOn()) != SH2_OK) {
        char msg[200];
        snprintf(msg, 200, "Could not turn sensor hub on. code %d\n", code);
        napi_throw_error(env, ERROR_INTERACTING_WITH_DRIVER, msg);
    }
    return NULL;
}

napi_value cb_devReset(napi_env env, napi_callback_info info) {
    if (sh2_devReset() != SH2_OK) {
        napi_throw_error(env, ERROR_INTERACTING_WITH_DRIVER,
                         "Could not reset the sensor hub.");
    };
    return NULL;
}

napi_value cb_devSleep(napi_env env, napi_callback_info info) {
    int code = sh2_devSleep();
    if (code != SH2_OK) {
        napi_throw_error(env, ERROR_INTERACTING_WITH_DRIVER,
                         "Couldn't put the hub into sleep.");
    }
    return NULL;
}

napi_value cb_setFrs(napi_env env, napi_callback_info info) {
    uint16_t recordId; // Which record to set.
    uint16_t words;    // Number of 32-bit words to write or 0 to delete record.
    void *data;        // Pointer to the buffer to write.
    size_t data_len;

    // Get arguments
    size_t argc = 2; // Record ID to write and the buffer for it.
    napi_value argv[2];

    napi_status status = napi_get_cb_info(env, info, &argc, argv, NULL, NULL);
    if (status != napi_ok) {
        napi_throw_error(env, ARGUMENT_ERROR, "Error setting frs");
        return NULL;
    }
    if (argc != 2) {
        napi_throw_error(env, ARGUMENT_ERROR, "Exactly 2 arguments expected.");
        return NULL;
    }

    // Read the buffer to be passed for sh2_setFrs(..)
    status = napi_get_buffer_info(env, argv[1], &data, &data_len);
    if (status != napi_ok) {
        napi_throw_error(env, ARGUMENT_ERROR,
                         "Second argument must be a buffer.");
        return NULL;
    }
    if (data_len % 4) {
        napi_throw_error(env, ARGUMENT_ERROR,
                         "Invalid buffer. Is it from getFrs()?");
        return NULL;
    }
    words = data_len / 4;

    // Get the record id to write
    uint32_t tmp;
    status = napi_get_value_uint32(env, argv[0], &tmp);
    if (status != napi_ok) {
        napi_throw_error(env, ARGUMENT_ERROR,
                         "First argument must be number representing the FRS "
                         "record to be written.");
        return NULL;
    }
    recordId = tmp;

    // Write the record
    int code = sh2_setFrs(recordId, data, words);
    if (code != SH2_OK) {
        napi_throw_error(
            env, UNKNOWN_ERROR,
            "Unknown error. Do the designated record and the buffer match?");
        return NULL;
    }

    return NULL; // No throw; OK.
}

napi_value cb_getFrs(napi_env env, napi_callback_info info) {
    size_t argc = 1;
    napi_value argv[1];

    napi_status status = napi_get_cb_info(env, info, &argc, argv, NULL, NULL);
    if (status != napi_ok) {
        napi_throw_error(env, UNKNOWN_ERROR, "Couldn't parse arguments.");
        return NULL;
    }
    if (argc != 1) {
        napi_throw_error(env, ARGUMENT_ERROR,
                         "Expected exactly one number argument designating the "
                         "setting to fetch.");
        return NULL;
    }

    uint32_t _recordId;
    uint16_t recordId;
    status = napi_get_value_uint32(env, argv[0], &_recordId);
    if (status != napi_ok) {
        napi_throw_error(env, ARGUMENT_ERROR, "Expected recordId argument.");
        return NULL;
    }
    recordId = _recordId;

    uint32_t data[300];
    uint16_t words = 75; // 75 * 4 = 300
    int code = sh2_getFrs(recordId, data, &words);
    if (code != SH2_OK) {
        napi_throw_error(env, UNKNOWN_ERROR,
                         "An error happened while fetching FRS data.");
        return NULL;
    }

    // Create Buffer for it
    napi_value buf;
    void *void_buf;
    status = napi_create_buffer_copy(env, words * 4, data, &void_buf, &buf);
    if (status != napi_ok) {
        napi_throw_error(env, ERROR_CREATING_NAPI_VALUE,
                         "Unable to create napi Buffer.");
        return NULL;
    }

    return buf; // No throw; OK!
}
