#include <endian.h>
#include <node/node_api.h>
#include <stdint.h>

#include "error.h"

uint8_t add_getters_to_accelerometer_report(napi_env env, napi_value report) {
    napi_value report_buf;

    // Get buffer from the report
    napi_status status =
        napi_get_named_property(env, report, "report", &report_buf);
    if (status != napi_ok) {
        napi_throw_error(env, NULL,
                         "Couldn't get named prop `report` from sensor event.");
        return 1;
    }
    void *data;
    size_t len;
    status = napi_get_buffer_info(env, report_buf, &data, &len);
    if (status != napi_ok) {
        napi_throw_error(env, NULL, "Couldn't get buffer from sensor event.");
        return 1;
    }

    // Calculate x/y/z acceleration values.
    uint16_t x_raw = le16toh(((int16_t *)data)[2]);
    uint16_t y_raw = le16toh(((int16_t *)data)[3]);
    uint16_t z_raw = le16toh(((int16_t *)data)[4]);

    int16_t x_ = x_raw;
    int16_t y_ = y_raw;
    int16_t z_ = z_raw;

    double x, y, z;
    x = (double)x_ / (1u << 8); // Q-point is 8
    y = (double)y_ / (1u << 8);
    z = (double)z_ / (1u << 8);

    napi_value X, Y, Z;

    // Create getters for acceleration values on different axes from the buffer
    status = napi_create_double(env, x, &X);
    if (status != napi_ok) {
        napi_throw_error(env, NULL,
                         "Couldn't create X-value for accelerometer report.");
        return 1;
    }

    status = napi_create_double(env, y, &Y);
    if (status != napi_ok) {
        napi_throw_error(env, NULL,
                         "Couldn't create Y-value for accelerometer report.");
        return 1;
    }

    status = napi_create_double(env, z, &Z);
    if (status != napi_ok) {
        napi_throw_error(env, NULL,
                         "Couldn't create X-value for accelerometer report.");
        return 1;
    }

    // Assign the getters to the report
    status = napi_set_named_property(env, report, "x", X);
    if (status != napi_ok) {
        napi_throw_error(env, NULL,
                         "Couldn't set x-getter for accelerometer report.");
        return 1;
    }
    status = napi_set_named_property(env, report, "y", Y);
    if (status != napi_ok) {
        napi_throw_error(env, NULL,
                         "Couldn't set y-getter for accelerometer report.");
        return 1;
    }
    status = napi_set_named_property(env, report, "z", Z);
    if (status != napi_ok) {
        napi_throw_error(env, NULL,
                         "Couldn't set z-getter for accelerometer report.");
        return 1;
    }
    return 0; // OK
}
