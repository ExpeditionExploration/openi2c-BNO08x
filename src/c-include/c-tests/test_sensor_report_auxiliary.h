#ifndef TEST_SENSOR_REPORT_AUXILIARY_H
#define TEST_SENSOR_REPORT_AUXILIARY_H

#include "sensor_report_auxialiry_fns.h"

/**
 * Add x,y,z properties to the returned JavaScript object.
 * Assertions are done in the Jest test file.
 */
napi_value test_add_xyz_to_sensor_report(napi_env env, napi_callback_info info);

/**
 * Add i, j, k, real and pitch, yaw, roll to the JavaScript object.
 * Assertions are done in the Jest test file.
 */
napi_value test_add_pitch_yaw_roll_to_rotation_vector(napi_env env,
                                                      napi_callback_info info);

#endif
