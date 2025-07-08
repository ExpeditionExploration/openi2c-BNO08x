#include <endian.h>
#include <node/node_api.h>
#include <stdint.h>

#include "error.h"
#include "sh2/euler.h"

uint8_t add_xyz_to_sensor_report(napi_env env, napi_value report) {
    napi_value report_buf;

    // Get buffer from the report
    napi_status status =
        napi_get_named_property(env, report, "report", &report_buf);
    if (status != napi_ok) {
        napi_throw_error(env, SENSOR_REPORT_ERROR,
                         "Couldn't get named prop `report` from sensor event.");
        return 1;
    }
    void *data;
    size_t len;
    status = napi_get_buffer_info(env, report_buf, &data, &len);
    if (status != napi_ok) {
        napi_throw_error(env, SENSOR_REPORT_ERROR,
                         "Couldn't get buffer from sensor event.");
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

    // Create props for acceleration values on different axes from the buffer
    status = napi_create_double(env, x, &X);
    if (status != napi_ok) {
        napi_throw_error(env, SENSOR_REPORT_ERROR,
                         "Couldn't create X-value for accelerometer report.");
        return 1;
    }

    status = napi_create_double(env, y, &Y);
    if (status != napi_ok) {
        napi_throw_error(env, SENSOR_REPORT_ERROR,
                         "Couldn't create Y-value for accelerometer report.");
        return 1;
    }

    status = napi_create_double(env, z, &Z);
    if (status != napi_ok) {
        napi_throw_error(env, SENSOR_REPORT_ERROR,
                         "Couldn't create X-value for accelerometer report.");
        return 1;
    }

    // Assign the getters to the report
    status = napi_set_named_property(env, report, "x", X);
    if (status != napi_ok) {
        napi_throw_error(env, SENSOR_REPORT_ERROR,
                         "Couldn't set x-getter for accelerometer report.");
        return 1;
    }
    status = napi_set_named_property(env, report, "y", Y);
    if (status != napi_ok) {
        napi_throw_error(env, SENSOR_REPORT_ERROR,
                         "Couldn't set y-getter for accelerometer report.");
        return 1;
    }
    status = napi_set_named_property(env, report, "z", Z);
    if (status != napi_ok) {
        napi_throw_error(env, SENSOR_REPORT_ERROR,
                         "Couldn't set z-getter for accelerometer report.");
        return 1;
    }
    return 0; // OK
}

uint8_t add_pitch_yaw_roll_to_rotation_vector(napi_env env, napi_value report) {
    napi_value report_buf;

    // Get buffer from the report
    napi_status status =
        napi_get_named_property(env, report, "report", &report_buf);
    if (status != napi_ok) {
        napi_throw_error(env, SENSOR_REPORT_ERROR,
                         "Couldn't get named prop `report` from sensor event.");
        return 1;
    }
    void *data;
    size_t len;
    status = napi_get_buffer_info(env, report_buf, &data, &len);
    if (status != napi_ok) {
        napi_throw_error(env, SENSOR_REPORT_ERROR,
                         "Couldn't get buffer from sensor event.");
        return 1;
    }

    // Calculate x/y/z acceleration values.
    uint16_t i_raw = le16toh(((int16_t *)data)[2]);
    uint16_t j_raw = le16toh(((int16_t *)data)[3]);
    uint16_t k_raw = le16toh(((int16_t *)data)[4]);
    uint16_t real_raw = le16toh(((int16_t *)data)[5]);

    int16_t i_ = i_raw;
    int16_t j_ = j_raw;
    int16_t k_ = k_raw;
    int16_t real_ = real_raw;

    double i, j, k, real;
    i = (double)i_ / (1u << 14); // Q-point is 14
    j = (double)j_ / (1u << 14);
    k = (double)k_ / (1u << 14);
    real = (double)real_ / (1u << 14);

    napi_value I, J, K, REAL;
    // Create prop values for quaternion components
    status = napi_create_double(env, i, &I);
    if (status != napi_ok) {
        napi_throw_error(env, SENSOR_REPORT_ERROR,
                         "Couldn't create i-value for quaternion.");
        return 1;
    }
    status = napi_create_double(env, j, &J);
    if (status != napi_ok) {
        napi_throw_error(env, SENSOR_REPORT_ERROR,
                         "Couldn't create j-value for quaternion.");
        return 1;
    }
    status = napi_create_double(env, k, &K);
    if (status != napi_ok) {
        napi_throw_error(env, SENSOR_REPORT_ERROR,
                         "Couldn't create k-value for quaternion.");
        return 1;
    }
    status = napi_create_double(env, real, &REAL);
    if (status != napi_ok) {
        napi_throw_error(env, SENSOR_REPORT_ERROR,
                         "Couldn't create real-value for quaternion.");
        return 1;
    }

    // Create prop values for yaw, pitch and roll.
    napi_value yaw, pitch, roll;
    float _yaw, _pitch, _roll;
    q_to_ypr(real, i, j, k, &_roll, &_pitch, &_yaw);
    status = napi_create_double(env, _yaw, &yaw);
    if (status != napi_ok) {
        napi_throw_error(env, SENSOR_REPORT_ERROR,
                         "Couldn't create napi_value for yaw.");
        return 1;
    }
    status = napi_create_double(env, _pitch, &pitch);
    if (status != napi_ok) {
        napi_throw_error(env, SENSOR_REPORT_ERROR,
                         "Couldn't create napi_value for pitch.");
        return 1;
    }
    status = napi_create_double(env, _roll, &roll);
    if (status != napi_ok) {
        napi_throw_error(env, SENSOR_REPORT_ERROR,
                         "Couldn't create napi_value for roll.");
        return 1;
    }

    // Attach quaternion props to the report.
    status = napi_set_named_property(env, report, "i", I);
    if (status != napi_ok) {
        napi_throw_error(env, SENSOR_REPORT_ERROR,
                         "Couldn't set named prop of quaternion i-component");
        return 1;
    }
    status = napi_set_named_property(env, report, "j", J);
    if (status != napi_ok) {
        napi_throw_error(env, SENSOR_REPORT_ERROR,
                         "Couldn't set named prop of quaternion j-component");
        return 1;
    }
    status = napi_set_named_property(env, report, "k", K);
    if (status != napi_ok) {
        napi_throw_error(env, SENSOR_REPORT_ERROR,
                         "Couldn't set named prop of quaternion k-component");
        return 1;
    }
    status = napi_set_named_property(env, report, "real", REAL);
    if (status != napi_ok) {
        napi_throw_error(
            env, SENSOR_REPORT_ERROR,
            "Couldn't set named prop of quaternion real-component");
        return 1;
    }

    // Attach euler angle props to the report.
    status = napi_set_named_property(env, report, "yaw", yaw);
    if (status != napi_ok) {
        napi_throw_error(env, SENSOR_REPORT_ERROR,
                         "Couldn't set named prop of yaw.");
        return 1;
    }
    status = napi_set_named_property(env, report, "pitch", pitch);
    if (status != napi_ok) {
        napi_throw_error(env, SENSOR_REPORT_ERROR,
                         "Couldn't set named prop of pitch");
        return 1;
    }
    status = napi_set_named_property(env, report, "roll", roll);
    if (status != napi_ok) {
        napi_throw_error(env, SENSOR_REPORT_ERROR,
                         "Couldn't set named prop of roll");
        return 1;
    }

    return 0; // OK
}
