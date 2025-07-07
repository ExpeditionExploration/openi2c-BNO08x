#include "c-tests/test_sensor_report_auxiliary.h"

#include <node/node_api.h>
#include <stdint.h>

#include "error.h"
#include "sensor_report_auxialiry_fns.h"

napi_value test_add_properties_to_acceleration_report(napi_env env,
                                                      napi_callback_info info) {
    napi_value buffer;
    uint8_t raw_buffer_data[11] = {
        0x01,       // Accelerometer report
        0x00,       // Sequence number
        0x00,       // Status
        0x00,       // Delay
        0xFF, 0x00, // X-axis LSB and MSB. This should translate to 255 as
                    // integer and to 0.99609375 as a Q8 fixed point value.
        0xFF, 0x00, // Y-axis the same
        0xFF, 0x00, // Z-axis the same
        0x00,       // Just to prevent segfault in case of off-by-one
    };
    napi_status status =
        napi_create_buffer_copy(env, 11, raw_buffer_data, NULL, &buffer);
    if (status != napi_ok) {
        napi_throw_error(env, ERROR_EXECUTING_TEST,
                         "Couldn't create test buffer.");
    }

    // Make an object for the buffer
    napi_value result;
    status = napi_create_object(env, &result);
    if (status != napi_ok) {
        napi_throw_error(env, ERROR_EXECUTING_TEST,
                         "Couldn't create test object.");
        return NULL;
    }

    // Put the buffer into the test object
    status = napi_set_named_property(env, result, "report", buffer);
    if (status != napi_ok) {
        napi_throw_error(env, ERROR_EXECUTING_TEST,
                         "Couldn't set property for the test object.");
        return NULL;
    }

    // Finally call the function to be tested
    uint8_t code = add_properties_to_acceleration_report(env, result);
    if (code) {
        napi_throw_error(env, ERROR_EXECUTING_TEST,
                         "Tested fn returned with error.");
        return NULL;
    }

    return result;
}

napi_value test_add_properties_to_rotation_vector(napi_env env,
                                                  napi_callback_info info) {
    napi_value buffer;
    uint8_t raw_buffer_data[14] = {
        0x05,       // Rotation vector report
        0x00,       // Sequence number
        0x00,       // Status
        0x00,       // Delay
        0xFF, 0x00, // Unit quaternion i-component. LSB 1st, and then MSB.
                    // Q-point is at 14, and the value is 0.0155639648438.
        0xFF, 0x00, // j-component the same
        0xFF, 0x00, // k-component the same
        0xFF, 0x00, // real-component the same
        0x00, 0x00, // Accuracy estimate
    };
    napi_status status =
        napi_create_buffer_copy(env, 14, raw_buffer_data, NULL, &buffer);
    if (status != napi_ok) {
        napi_throw_error(env, ERROR_EXECUTING_TEST,
                         "Couldn't create test buffer.");
    }

    // Make an object for the buffer
    napi_value result;
    status = napi_create_object(env, &result);
    if (status != napi_ok) {
        napi_throw_error(env, ERROR_EXECUTING_TEST,
                         "Couldn't create test object.");
        return NULL;
    }

    // Put the buffer into the test object
    status = napi_set_named_property(env, result, "report", buffer);
    if (status != napi_ok) {
        napi_throw_error(env, ERROR_EXECUTING_TEST,
                         "Couldn't set property for the test object.");
        return NULL;
    }

    // Finally call the function to be tested
    uint8_t code = add_properties_to_rotation_vector(env, result);
    if (code) {
        napi_throw_error(env, ERROR_EXECUTING_TEST,
                         "Tested fn returned with error.");
        return NULL;
    }

    return result;
}
