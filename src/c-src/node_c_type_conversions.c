
#include <limits.h>
#include <node/node_api.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "error.h"
#include "sensor_report_auxialiry_fns.h"
#include "sh2/sh2.h"

void free_event(napi_env env, void* finalize_data, void* finalize_hint) {
    if (finalize_data != NULL) { free(finalize_data); }
}
napi_value c_to_SensorEvent(napi_env env, sh2_SensorEvent_t* ev) {
    napi_status status;
    napi_value obj; // JS object
    status = napi_create_object(env, &obj);

    uint8_t* buf = malloc(ev->len);
    if (buf == NULL) {
        napi_throw_error(env, ERROR_TRANSLATING_STRUCT_TO_NODE,
                         "Failed to allocate memory for event");
        return NULL;
    }
    memcpy((void*)buf, (const void*)&ev->report, ev->len);

    napi_value timestamp_uS; // uint64_t -> number
    napi_value delay_uS;     // int64_t -> number
    napi_value len;          // uint8_t -> number
    napi_value reportId;     // uint8_t -> number
    napi_value report;       // uint8_t -> ArrayBuffer
    status |= napi_create_bigint_uint64(env, ev->timestamp_uS, &timestamp_uS);
    status |= napi_create_int64(env, ev->delay_uS, &delay_uS);
    status |= napi_create_uint32(env, ev->len, &len);
    status |= napi_create_uint32(env, ev->reportId, &reportId);
    status |= napi_create_external_buffer(env, ev->len, buf, free_event, NULL,
                                          &report);
    if (status != napi_ok) {
        napi_throw_error(env, ERROR_TRANSLATING_STRUCT_TO_NODE,
                         "Failed to create NAPI value");
        return NULL;
    }

    status = napi_set_named_property(env, obj, "timestampMicroseconds",
                                     timestamp_uS);
    status |= napi_set_named_property(env, obj, "delayMicroseconds", delay_uS);
    status |= napi_set_named_property(env, obj, "length", len);
    status |= napi_set_named_property(env, obj, "reportId", reportId);
    status |= napi_set_named_property(env, obj, "report", report);
    if (status != napi_ok) {
        napi_throw_error(env, ERROR_TRANSLATING_STRUCT_TO_NODE,
                         "Failed to set NAPI property");
        return NULL;
    }

    // Finally set getters for the actual sensor values.
    uint8_t ret;
    switch (ev->reportId) {
        case SH2_ACCELEROMETER:
        case SH2_LINEAR_ACCELERATION:
        case SH2_GRAVITY:
        case SH2_RAW_MAGNETOMETER:
        case SH2_MAGNETIC_FIELD_UNCALIBRATED:
        case SH2_MAGNETIC_FIELD_CALIBRATED:
        case SH2_GYROSCOPE_UNCALIBRATED:
        case SH2_GYROSCOPE_CALIBRATED:
            ret = add_properties_to_acceleration_report(env, obj);
            if (ret != 0) {
                // add_properties_* already throws napi error
                return NULL;
            }
            break;
        case SH2_ROTATION_VECTOR:
            ret = add_properties_to_rotation_vector(env, obj);
            if (ret != 0) {
                // add_properties_to_* already throws napi error
                return NULL;
            }
    }

    return obj;
}

napi_value c_to_ProductId(napi_env env, sh2_ProductId_t* id) {
    napi_status status;
    napi_value obj; // JS object
    status = napi_create_object(env, &obj);

    napi_value resetCause;     // uint8_t -> number
    napi_value swVersionMajor; // uint8_t -> number
    napi_value swVersionMinor; // uint8_t -> number
    napi_value swPartNumber;   // uint32_t -> number
    napi_value swBuildNumber;  // uint32_t -> number
    napi_value swVersionPatch; // uint16_t -> number
    napi_value reserved0;      // uint8_t -> number
    napi_value reserved1;      // uint8_t -> number

    status |= napi_create_uint32(env, id->resetCause, &resetCause);
    status |= napi_create_uint32(env, id->swVersionMajor, &swVersionMajor);
    status |= napi_create_uint32(env, id->swVersionMinor, &swVersionMinor);
    status |= napi_create_uint32(env, id->swPartNumber, &swPartNumber);
    status |= napi_create_uint32(env, id->swBuildNumber, &swBuildNumber);
    status |= napi_create_uint32(env, id->swVersionPatch, &swVersionPatch);
    status |= napi_create_uint32(env, id->reserved0, &reserved0);
    status |= napi_create_uint32(env, id->reserved1, &reserved1);

    if (status != napi_ok) {
        napi_throw_error(env, ERROR_TRANSLATING_STRUCT_TO_NODE,
                         "Couldn't construct a ProductId");
    }

    return obj;
}

napi_value c_to_SensorIdEnum(napi_env env) {
    napi_status status;
    napi_value obj;
    status = napi_create_object(env, &obj);

    napi_value raw_accelerometer;
    napi_value accelerometer;
    napi_value linear_acceleration;
    napi_value gravity;
    napi_value raw_gyroscope;
    napi_value gyroscope_calibrated;
    napi_value gyroscope_uncalibrated;
    napi_value raw_magnetometer;
    napi_value magnetic_field_calibrated;
    napi_value magnetic_field_uncalibrated;
    napi_value rotation_vector;
    napi_value game_rotation_vector;
    napi_value geomagnetic_rotation_vector;
    napi_value pressure;
    napi_value ambient_light;
    napi_value humidity;
    napi_value proximity;
    napi_value temperature;
    napi_value reserved;
    napi_value tap_detector;
    napi_value step_detector;
    napi_value step_counter;
    napi_value significant_motion;
    napi_value stability_classifier;
    napi_value shake_detector;
    napi_value flip_detector;
    napi_value pickup_detector;
    napi_value stability_detector;
    napi_value personal_activity_classifier;
    napi_value sleep_detector;
    napi_value tilt_detector;
    napi_value pocket_detector;
    napi_value circle_detector;
    napi_value heart_rate_monitor;
    napi_value arvr_stabilized_rv;
    napi_value arvr_stabilized_grv;
    napi_value gyro_integrated_rv;
    napi_value izro_motion_request;
    napi_value raw_optical_flow;
    napi_value dead_reckoning_pose;
    napi_value wheel_encoder;
    napi_value max_sensor_id;

    status |=
        napi_create_uint32(env, SH2_RAW_ACCELEROMETER, &raw_accelerometer);
    status |= napi_create_uint32(env, SH2_ACCELEROMETER, &accelerometer);
    status |=
        napi_create_uint32(env, SH2_LINEAR_ACCELERATION, &linear_acceleration);
    status |= napi_create_uint32(env, SH2_GRAVITY, &gravity);
    status |= napi_create_uint32(env, SH2_RAW_GYROSCOPE, &raw_gyroscope);
    status |= napi_create_uint32(env, SH2_GYROSCOPE_CALIBRATED,
                                 &gyroscope_calibrated);
    status |= napi_create_uint32(env, SH2_GYROSCOPE_UNCALIBRATED,
                                 &gyroscope_uncalibrated);
    status |= napi_create_uint32(env, SH2_RAW_MAGNETOMETER, &raw_magnetometer);
    status |= napi_create_uint32(env, SH2_MAGNETIC_FIELD_CALIBRATED,
                                 &magnetic_field_calibrated);
    status |= napi_create_uint32(env, SH2_MAGNETIC_FIELD_UNCALIBRATED,
                                 &magnetic_field_uncalibrated);
    status |= napi_create_uint32(env, SH2_ROTATION_VECTOR, &rotation_vector);
    status |= napi_create_uint32(env, SH2_GAME_ROTATION_VECTOR,
                                 &game_rotation_vector);
    status |= napi_create_uint32(env, SH2_GEOMAGNETIC_ROTATION_VECTOR,
                                 &geomagnetic_rotation_vector);
    status |= napi_create_uint32(env, SH2_PRESSURE, &pressure);
    status |= napi_create_uint32(env, SH2_AMBIENT_LIGHT, &ambient_light);
    status |= napi_create_uint32(env, SH2_HUMIDITY, &humidity);
    status |= napi_create_uint32(env, SH2_PROXIMITY, &proximity);
    status |= napi_create_uint32(env, SH2_TEMPERATURE, &temperature);
    status |= napi_create_uint32(env, SH2_RESERVED, &reserved);
    status |= napi_create_uint32(env, SH2_TAP_DETECTOR, &tap_detector);
    status |= napi_create_uint32(env, SH2_STEP_DETECTOR, &step_detector);
    status |= napi_create_uint32(env, SH2_STEP_COUNTER, &step_counter);
    status |=
        napi_create_uint32(env, SH2_SIGNIFICANT_MOTION, &significant_motion);
    status |= napi_create_uint32(env, SH2_STABILITY_CLASSIFIER,
                                 &stability_classifier);
    status |= napi_create_uint32(env, SH2_SHAKE_DETECTOR, &shake_detector);
    status |= napi_create_uint32(env, SH2_FLIP_DETECTOR, &flip_detector);
    status |= napi_create_uint32(env, SH2_PICKUP_DETECTOR, &pickup_detector);
    status |=
        napi_create_uint32(env, SH2_STABILITY_DETECTOR, &stability_detector);
    status |= napi_create_uint32(env, SH2_PERSONAL_ACTIVITY_CLASSIFIER,
                                 &personal_activity_classifier);
    status |= napi_create_uint32(env, SH2_SLEEP_DETECTOR, &sleep_detector);
    status |= napi_create_uint32(env, SH2_TILT_DETECTOR, &tilt_detector);
    status |= napi_create_uint32(env, SH2_POCKET_DETECTOR, &pocket_detector);
    status |= napi_create_uint32(env, SH2_CIRCLE_DETECTOR, &circle_detector);
    status |=
        napi_create_uint32(env, SH2_HEART_RATE_MONITOR, &heart_rate_monitor);
    status |=
        napi_create_uint32(env, SH2_ARVR_STABILIZED_RV, &arvr_stabilized_rv);
    status |=
        napi_create_uint32(env, SH2_ARVR_STABILIZED_GRV, &arvr_stabilized_grv);
    status |=
        napi_create_uint32(env, SH2_GYRO_INTEGRATED_RV, &gyro_integrated_rv);
    status |=
        napi_create_uint32(env, SH2_IZRO_MOTION_REQUEST, &izro_motion_request);
    status |= napi_create_uint32(env, SH2_RAW_OPTICAL_FLOW, &raw_optical_flow);
    status |=
        napi_create_uint32(env, SH2_DEAD_RECKONING_POSE, &dead_reckoning_pose);
    status |= napi_create_uint32(env, SH2_WHEEL_ENCODER, &wheel_encoder);
    status |= napi_create_uint32(env, SH2_MAX_SENSOR_ID, &max_sensor_id);

    status |= napi_set_named_property(env, obj, "RAW_ACCELEROMETER",
                                      raw_accelerometer);
    status |= napi_set_named_property(env, obj, "ACCELEROMETER", accelerometer);
    status |= napi_set_named_property(env, obj, "LINEAR_ACCELERATION",
                                      linear_acceleration);
    status |= napi_set_named_property(env, obj, "GRAVITY", gravity);
    status |= napi_set_named_property(env, obj, "RAW_GYROSCOPE", raw_gyroscope);
    status |= napi_set_named_property(env, obj, "MAGNETIC_FIELD_CALIBRATED",
                                      magnetic_field_calibrated);
    status |= napi_set_named_property(env, obj, "MAGNETIC_FIELD_UNCALIBRATED",
                                      magnetic_field_uncalibrated);
    status |=
        napi_set_named_property(env, obj, "ROTATION_VECTOR", rotation_vector);
    status |= napi_set_named_property(env, obj, "GAME_ROTATION_VECTOR",
                                      game_rotation_vector);
    status |= napi_set_named_property(env, obj, "GEOMAGNETIC_ROTATION_VECTOR",
                                      geomagnetic_rotation_vector);
    status |= napi_set_named_property(env, obj, "PRESSURE", pressure);
    status |= napi_set_named_property(env, obj, "AMBIENT_LIGHT", ambient_light);
    status |= napi_set_named_property(env, obj, "HUMIDITY", humidity);
    status |= napi_set_named_property(env, obj, "PROXIMITY", proximity);
    status |= napi_set_named_property(env, obj, "TEMPERATURE", temperature);
    status |= napi_set_named_property(env, obj, "RESERVED", reserved);
    status |= napi_set_named_property(env, obj, "TAP_DETECTOR", tap_detector);
    status |= napi_set_named_property(env, obj, "STEP_DETECTOR", step_detector);
    status |= napi_set_named_property(env, obj, "STEP_COUNTER", step_counter);
    status |= napi_set_named_property(env, obj, "SIGNIFICANT_MOTION",
                                      significant_motion);
    status |= napi_set_named_property(env, obj, "STABILITY_CLASSIFIER",
                                      stability_classifier);
    status |=
        napi_set_named_property(env, obj, "SHAKE_DETECTOR", shake_detector);
    status |= napi_set_named_property(env, obj, "FLIP_DETECTOR", flip_detector);
    status |=
        napi_set_named_property(env, obj, "PICKUP_DETECTOR", pickup_detector);
    status |= napi_set_named_property(env, obj, "STABILITY_DETECTOR",
                                      stability_detector);
    status |= napi_set_named_property(env, obj, "PERSONAL_ACTIVITY_CLASSIFIER",
                                      personal_activity_classifier);
    status |=
        napi_set_named_property(env, obj, "SLEEP_DETECTOR", sleep_detector);
    status |= napi_set_named_property(env, obj, "TILT_DETECTOR", tilt_detector);
    status |=
        napi_set_named_property(env, obj, "POCKET_DETECTOR", pocket_detector);
    status |=
        napi_set_named_property(env, obj, "CIRCLE_DETECTOR", circle_detector);
    status |= napi_set_named_property(env, obj, "HEART_RATE_MONITOR",
                                      heart_rate_monitor);
    status |= napi_set_named_property(env, obj, "ARVR_STABILIZED_RV",
                                      arvr_stabilized_rv);
    status |= napi_set_named_property(env, obj, "ARVR_STABILIZED_GRV",
                                      arvr_stabilized_grv);
    status |= napi_set_named_property(env, obj, "GYRO_INTEGRATED_RV",
                                      gyro_integrated_rv);
    status |= napi_set_named_property(env, obj, "IZRO_MOTION_REQUEST",
                                      izro_motion_request);
    status |=
        napi_set_named_property(env, obj, "RAW_OPTICAL_FLOW", raw_optical_flow);
    status |= napi_set_named_property(env, obj, "DEAD_RECKONING_POSE",
                                      dead_reckoning_pose);
    status |= napi_set_named_property(env, obj, "WHEEL_ENCODER", wheel_encoder);
    status |= napi_set_named_property(env, obj, "MAX_SENSOR_ID", max_sensor_id);

    if (status != napi_ok) {
        napi_throw_error(env, ERROR_TRANSLATING_STRUCT_TO_NODE,
                         "Could not create SensorId enum.");
        return NULL;
    }

    return obj;
}

napi_value c_to_SensorConfig(napi_env env, sh2_SensorConfig_t* cfg) {
    napi_status status;
    napi_value obj;
    status = napi_create_object(env, &obj);

    napi_value changeSensitivityEnabled;
    napi_value changeSensitivityRelative;
    napi_value wakeupEnabled;
    napi_value alwaysOnEnabled;
    napi_value sniffEnabled;
    napi_value changeSensitivity;
    napi_value reportInterval_us;
    napi_value batchInterval_us;
    napi_value sensorSpecific;

    // Create fields
    status |= napi_get_boolean(env, cfg->changeSensitivityEnabled,
                               &changeSensitivityEnabled);
    status |= napi_get_boolean(env, cfg->changeSensitivityRelative,
                               &changeSensitivityRelative);
    status |= napi_get_boolean(env, cfg->wakeupEnabled, &wakeupEnabled);
    status |= napi_get_boolean(env, cfg->sniffEnabled, &sniffEnabled);
    status |= napi_get_boolean(env, cfg->alwaysOnEnabled, &alwaysOnEnabled);
    status |=
        napi_create_uint32(env, cfg->changeSensitivity, &changeSensitivity);
    status |=
        napi_create_uint32(env, cfg->reportInterval_us, &reportInterval_us);
    status |= napi_create_uint32(env, cfg->batchInterval_us, &batchInterval_us);
    status |= napi_create_uint32(env, cfg->sensorSpecific, &sensorSpecific);

    // Set named props
    status |= napi_set_named_property(env, obj, "changeSensitivityEnabled",
                                      changeSensitivityEnabled);
    status |= napi_set_named_property(env, obj, "changeSensitivityRelative",
                                      changeSensitivityRelative);
    status |= napi_set_named_property(env, obj, "wakeupEnabled", wakeupEnabled);
    status |=
        napi_set_named_property(env, obj, "alwaysOnEnabled", alwaysOnEnabled);
    status |= napi_set_named_property(env, obj, "sniffEnabled", sniffEnabled);
    status |= napi_set_named_property(env, obj, "changeSensitivity",
                                      changeSensitivity);
    status |= napi_set_named_property(env, obj, "reportInterval_us",
                                      reportInterval_us);
    status |=
        napi_set_named_property(env, obj, "batchInterval_us", batchInterval_us);
    status |=
        napi_set_named_property(env, obj, "sensorSpecific", sensorSpecific);

    if (status != napi_ok) {
        napi_throw_error(env, ERROR_TRANSLATING_STRUCT_TO_NODE,
                         "Couldn't construct a SensorConfig.");
        return NULL;
    }

    return obj;
}

void free_meta(napi_env env, void* finalize_data, void* finalize_hint) {
    if (finalize_data != NULL || (uint8_t*)finalize_data != 0) {
        free(finalize_data);
    }
}
napi_value c_to_SensorMetadata(napi_env env, sh2_SensorMetadata_t* meta) {
    napi_status status;
    napi_value obj;
    status = napi_create_object(env, &obj);

    napi_value meVersion;        /**< @brief Motion Engine Version */
    napi_value mhVersion;        /**< @brief Motion Hub Version */
    napi_value shVersion;        /**< @brief SensorHub Version */
    napi_value range;            /**< @brief Same units as sensor reports */
    napi_value resolution;       /**< @brief Same units as sensor reports */
    napi_value revision;         /**< @brief Metadata record format revision */
    napi_value power_mA;         /**< @brief [mA] Fixed point 16Q10 format */
    napi_value minPeriod_uS;     /**< @brief [uS] */
    napi_value maxPeriod_uS;     /**< @brief [uS] */
    napi_value fifoReserved;     /**< @brief (Unused) */
    napi_value fifoMax;          /**< @brief (Unused) */
    napi_value batchBufferBytes; /**< @brief (Unused) */
    napi_value qPoint1;          /**< @brief q point for sensor values */
    napi_value qPoint2; /**< @brief q point for accuracy or bias fields */
    napi_value
        qPoint3; /**< @brief q point for sensor data change sensitivity */
    napi_value vendorIdLen;       /**< @brief [bytes] */
    napi_value vendorId;          /**< @brief Vendor name and part number */
    napi_value sensorSpecificLen; /**< @brief [bytes] */
    napi_value sensorSpecific;    /**< @brief See SH-2 Reference Manual */

    // Create napi values
    status |= napi_create_uint32(env, meta->meVersion, &meVersion);
    status |= napi_create_uint32(env, meta->mhVersion, &mhVersion);
    status |= napi_create_uint32(env, meta->shVersion, &shVersion);
    status |= napi_create_uint32(env, meta->range, &range);
    status |= napi_create_uint32(env, meta->resolution, &resolution);
    status |= napi_create_uint32(env, meta->revision, &revision);
    status |= napi_create_uint32(env, meta->power_mA, &power_mA);
    status |= napi_create_uint32(env, meta->minPeriod_uS, &minPeriod_uS);
    status |= napi_create_uint32(env, meta->maxPeriod_uS, &maxPeriod_uS);
    status |= napi_create_uint32(env, meta->fifoReserved, &fifoReserved);
    status |= napi_create_uint32(env, meta->fifoMax, &fifoMax);
    status |=
        napi_create_uint32(env, meta->batchBufferBytes, &batchBufferBytes);
    status |= napi_create_uint32(env, meta->qPoint1, &qPoint1);
    status |= napi_create_uint32(env, meta->qPoint2, &qPoint2);
    status |= napi_create_uint32(env, meta->qPoint3, &qPoint3);
    status |= napi_create_uint32(env, meta->vendorIdLen, &vendorIdLen);
    status |=
        napi_create_uint32(env, meta->sensorSpecificLen, &sensorSpecificLen);
    status |= napi_create_string_utf8(env, meta->vendorId, meta->vendorIdLen,
                                      &vendorId);
    u_int8_t* buf = malloc(meta->sensorSpecificLen);
    if (buf == NULL) {
        napi_throw_error(env, ERROR_TRANSLATING_STRUCT_TO_NODE,
                         "Couldn't allocate memory for sensorSpecific.");
        return NULL;
    }
    memcpy(buf, &meta->sensorSpecific, meta->sensorSpecificLen);
    napi_status status2 =
        napi_create_external_buffer(env, meta->sensorSpecificLen, meta,
                                    free_meta, &status2, &sensorSpecific);

    // Set the objects props
    status |= napi_set_named_property(env, obj, "meVersion", meVersion);
    status |= napi_set_named_property(env, obj, "mhVersion", mhVersion);
    status |= napi_set_named_property(env, obj, "shVersion", shVersion);
    status |= napi_set_named_property(env, obj, "range", range);
    status |= napi_set_named_property(env, obj, "resolution", resolution);
    status |= napi_set_named_property(env, obj, "revision", revision);
    status |= napi_set_named_property(env, obj, "power_mA", power_mA);
    status |= napi_set_named_property(env, obj, "minPeriod_uS", minPeriod_uS);
    status |= napi_set_named_property(env, obj, "maxPeriod_uS", maxPeriod_uS);
    status |= napi_set_named_property(env, obj, "fifoReserved", fifoReserved);
    status |= napi_set_named_property(env, obj, "fifoMax", fifoMax);
    status |=
        napi_set_named_property(env, obj, "batchBufferBytes", batchBufferBytes);
    status |= napi_set_named_property(env, obj, "qPoint1", qPoint1);
    status |= napi_set_named_property(env, obj, "qPoint2", qPoint2);
    status |= napi_set_named_property(env, obj, "qPoint3", qPoint3);
    status |= napi_set_named_property(env, obj, "vendorIdLen", vendorIdLen);
    status |= napi_set_named_property(env, obj, "vendorId", vendorId);
    status |= napi_set_named_property(env, obj, "sensorSpecificLen",
                                      sensorSpecificLen);
    status |=
        napi_set_named_property(env, obj, "sensorSpecific", sensorSpecific);

    if (status != napi_ok) {
        napi_throw_error(env, ERROR_TRANSLATING_STRUCT_TO_NODE,
                         "Couldn't build SensorMetaData.");
        return NULL;
    }
    return obj;
}

napi_value c_to_ErrorRecord(napi_env env, sh2_ErrorRecord_t* err) {
    napi_status status;
    napi_value obj;
    status = napi_create_object(env, &obj);

    napi_value severity; /**< @brief Error severity, 0: most severe. */
    napi_value sequence; /**< @brief Sequence number (by severity) */
    napi_value source; /**< 1-MotionEngine, 2-MotionHub, 3-SensorHub, 4-Chip  */
    napi_value error;  /**< @brief See SH-2 Reference Manual */
    napi_value module; /**< @brief See SH-2 Reference Manual */
    napi_value code;   /**< @brief See SH-2 Reference Manual */

    status |= napi_create_uint32(env, err->severity, &severity);
    status |= napi_create_uint32(env, err->sequence, &sequence);
    status |= napi_create_uint32(env, err->source, &source);
    status |= napi_create_uint32(env, err->error, &error);
    status |= napi_create_uint32(env, err->module, &module);
    status |= napi_create_uint32(env, err->code, &code);

    status |= napi_set_named_property(env, obj, "severity", severity);
    status |= napi_set_named_property(env, obj, "sequence", sequence);
    status |= napi_set_named_property(env, obj, "source", source);
    status |= napi_set_named_property(env, obj, "serroreverity", error);
    status |= napi_set_named_property(env, obj, "smoduleeverity", module);
    status |= napi_set_named_property(env, obj, "scodeeverity", code);

    if (status != napi_ok) {
        napi_throw_error(env, ERROR_TRANSLATING_STRUCT_TO_NODE,
                         "Couldn't build an ErrorRecord.");
        return NULL;
    }

    return obj;
}

napi_value c_to_Counts(napi_env env, sh2_Counts_t* counts) {
    napi_value obj;
    napi_status status;
    status = napi_create_object(env, &obj);

    napi_value offered;
    napi_value accepted;
    napi_value on;
    napi_value attempted;

    status |= napi_create_uint32(env, counts->offered, &offered);
    status |= napi_create_uint32(env, counts->accepted, &accepted);
    status |= napi_create_uint32(env, counts->on, &on);
    status |= napi_create_uint32(env, counts->attempted, &attempted);

    status |= napi_set_named_property(env, obj, "offered", offered);
    status |= napi_set_named_property(env, obj, "accepted", accepted);
    status |= napi_set_named_property(env, obj, "on", on);
    status |= napi_set_named_property(env, obj, "attempted", attempted);

    if (status != napi_ok) {
        napi_throw_error(env, ERROR_TRANSLATING_STRUCT_TO_NODE,
                         "Couldn't build Counts object");
        return NULL;
    }
    return obj;
}

napi_value c_to_TareBasis(napi_env env) {
    napi_value obj;
    napi_status status;
    status = napi_create_object(env, &obj);

    napi_value tare_basis_rotation_vector;
    napi_value tare_basis_gaming_rotation_vector;
    napi_value tare_basis_geomagnetic_rotation_vector;

    status |= napi_create_uint32(env, SH2_TARE_BASIS_ROTATION_VECTOR,
                                 &tare_basis_rotation_vector);
    status |= napi_create_uint32(env, SH2_TARE_BASIS_GAMING_ROTATION_VECTOR,
                                 &tare_basis_gaming_rotation_vector);
    status |=
        napi_create_uint32(env, SH2_TARE_BASIS_GEOMAGNETIC_ROTATION_VECTOR,
                           &tare_basis_geomagnetic_rotation_vector);

    status |= napi_set_named_property(env, obj, "TARE_BASIS_ROTATION_VECTOR",
                                      tare_basis_gaming_rotation_vector);
    status |=
        napi_set_named_property(env, obj, "TARE_BASIS_GAMING_ROTATION_VECTOR",
                                tare_basis_gaming_rotation_vector);
    status |= napi_set_named_property(env, obj,
                                      "TARE_BASIS_GEOMAGNETIC_ROTATION_VECTOR",
                                      tare_basis_gaming_rotation_vector);

    if (status != napi_ok) {
        napi_throw_error(env, ERROR_TRANSLATING_STRUCT_TO_NODE,
                         "Could not construct object TareBasis.");
        return NULL;
    }
    return obj;
}

napi_value c_to_TareAxis(napi_env env) {
    napi_value obj;
    napi_status status;
    status = napi_create_object(env, &obj);

    napi_value tare_x;
    napi_value tare_y;
    napi_value tare_z;
    napi_value tare_control_vector_x;
    napi_value tare_control_vector_y;
    napi_value tare_control_vector_z;
    napi_value tare_sequence_default;
    napi_value tare_sequence_pre;
    napi_value tare_sequence_post;

    status |= napi_create_uint32(env, SH2_TARE_X, &tare_x);
    status |= napi_create_uint32(env, SH2_TARE_Y, &tare_y);
    status |= napi_create_uint32(env, SH2_TARE_Z, &tare_z);
    status |= napi_create_uint32(env, SH2_TARE_CONTROL_VECTOR_X,
                                 &tare_control_vector_x);
    status |= napi_create_uint32(env, SH2_TARE_CONTROL_VECTOR_Y,
                                 &tare_control_vector_y);
    status |= napi_create_uint32(env, SH2_TARE_CONTROL_VECTOR_Z,
                                 &tare_control_vector_z);
    status |= napi_create_uint32(env, SH2_TARE_CONTROL_SEQUENCE_DEFAULT,
                                 &tare_sequence_default);
    status |= napi_create_uint32(env, SH2_TARE_CONTROL_SEQUENCE_PRE,
                                 &tare_sequence_pre);
    status |= napi_create_uint32(env, SH2_TARE_CONTROL_SEQUENCE_POST,
                                 &tare_sequence_post);

    status |= napi_set_named_property(env, obj, "TARE_X", tare_x);
    status |= napi_set_named_property(env, obj, "SH2_TARE_Y", tare_y);
    status |= napi_set_named_property(env, obj, "SH2_TARE_Z", tare_z);
    status |= napi_set_named_property(env, obj, "SH2_TARE_CONTROL_VECTOR_X",
                                      tare_control_vector_x);
    status |= napi_set_named_property(env, obj, "SH2_TARE_CONTROL_VECTOR_Y",
                                      tare_control_vector_y);
    status |= napi_set_named_property(env, obj, "SH2_TARE_CONTROL_VECTOR_Z",
                                      tare_control_vector_z);
    status |= napi_set_named_property(
        env, obj, "SH2_TARE_CONTROL_SEQUENCE_DEFAULT", tare_sequence_default);
    status |= napi_set_named_property(env, obj, "SH2_TARE_CONTROL_SEQUENCE_PRE",
                                      tare_sequence_pre);
    status |= napi_set_named_property(
        env, obj, "SH2_TARE_CONTROL_SEQUENCE_POST", tare_sequence_post);

    if (status != napi_ok) {
        napi_throw_error(env, ERROR_TRANSLATING_STRUCT_TO_NODE,
                         "Error constructing object TareAxis.");
        return NULL;
    }
    return obj;
}

napi_value c_to_Quaternion(napi_env env, sh2_Quaternion_t* qt) {
    napi_value obj;
    napi_status status;
    status = napi_create_object(env, &obj);

    napi_value x, y, z, w;

    status |= napi_create_double(env, qt->x, &x);
    status |= napi_create_double(env, qt->y, &y);
    status |= napi_create_double(env, qt->z, &z);
    status |= napi_create_double(env, qt->w, &w);

    status |= napi_set_named_property(env, obj, "x", x);
    status |= napi_set_named_property(env, obj, "y", y);
    status |= napi_set_named_property(env, obj, "z", z);
    status |= napi_set_named_property(env, obj, "w", w);

    if (status != napi_ok) {
        napi_throw_error(env, ERROR_TRANSLATING_STRUCT_TO_NODE,
                         "Couldn't construct a Quaternion.");
        return NULL;
    }
    return obj;
}

napi_value c_to_MotionIntent(napi_env env) {
    napi_value obj;
    napi_status status;
    status = napi_create_object(env, &obj);

    napi_value izro_mi_unknown;
    napi_value izro_mi_stationary_no_vibration;
    napi_value izro_mi_stationary_with_vibration;
    napi_value izro_mi_in_motion;
    napi_value izro_mi_accelerating;

    status |= napi_create_uint32(env, (uint32_t)SH2_IZRO_MI_UNKNOWN,
                                 &izro_mi_unknown);
    status |=
        napi_create_uint32(env, (uint32_t)SH2_IZRO_MI_STATIONARY_NO_VIBRATION,
                           &izro_mi_stationary_no_vibration);
    status |=
        napi_create_uint32(env, (uint32_t)SH2_IZRO_MI_STATIONARY_WITH_VIBRATION,
                           &izro_mi_stationary_with_vibration);
    status |= napi_create_uint32(env, (uint32_t)SH2_IZRO_MI_IN_MOTION,
                                 &izro_mi_in_motion);
    status |= napi_create_uint32(env, (uint32_t)SH2_IZRO_MI_ACCELERATING,
                                 &izro_mi_accelerating);

    status |=
        napi_set_named_property(env, obj, "IZRO_MI_UNKNOWN", izro_mi_unknown);
    status |=
        napi_set_named_property(env, obj, "IZRO_MI_STATIONARY_NO_VIBRATION",
                                izro_mi_stationary_no_vibration);
    status |=
        napi_set_named_property(env, obj, "IZRO_MI_STATIONARY_WITH_VIBRATION",
                                izro_mi_stationary_with_vibration);
    status |= napi_set_named_property(env, obj, "IZRO_MI_IN_MOTION",
                                      izro_mi_in_motion);
    status |= napi_set_named_property(env, obj, "IZRO_MI_ACCELERATING",
                                      izro_mi_accelerating);

    if (status != napi_ok) {
        napi_throw_error(env, ERROR_TRANSLATING_STRUCT_TO_NODE,
                         "Couldn't create MotionIntent.");
        return NULL;
    }
    return obj;
}

napi_value c_to_AsyncEventId(napi_env env) {
    napi_value obj;
    napi_status status;
    status = napi_create_object(env, &obj);

    napi_value reset, shtp_event, get_feature_resp;

    napi_create_uint32(env, SH2_RESET, &reset);
    napi_create_uint32(env, SH2_SHTP_EVENT, &shtp_event);
    napi_create_uint32(env, SH2_GET_FEATURE_RESP, &get_feature_resp);

    napi_set_named_property(env, obj, "RESET", reset);
    napi_set_named_property(env, obj, "SHTP_EVENT", shtp_event);
    napi_set_named_property(env, obj, "GET_FEATURE_RESP", get_feature_resp);

    if (status != napi_ok) {
        napi_throw_error(env, ERROR_TRANSLATING_STRUCT_TO_NODE,
                         "couldn't construct AsyncEventId");
        return NULL;
    }
    return obj;
}

napi_value c_to_SensorConfigResp(napi_env env, sh2_SensorConfigResp_t* cfg) {
    napi_value obj;
    napi_status status;
    status = napi_create_object(env, &obj);

    napi_value sensorId;
    napi_value config;

    status |= napi_create_uint32(env, cfg->sensorId, &sensorId);
    config = c_to_SensorConfig(env, &cfg->sensorConfig);
    if (config == NULL) { return NULL; }
    status |= napi_set_named_property(env, obj, "sensorId", sensorId);
    status |= napi_set_named_property(env, obj, "config", config);
    if (status != napi_ok) {
        napi_throw_error(env, ERROR_TRANSLATING_STRUCT_TO_NODE,
                         "couldn't construct SensorConfigResp");
        return NULL;
    }
    return obj;
}

napi_value c_to_AsyncEvent(napi_env env, sh2_AsyncEvent_t* evt) {
    napi_value obj;
    napi_status status;
    status = napi_create_object(env, &obj);

    napi_value eventId;
    napi_value shtpEvent;
    napi_value sh2SensorConfigResp;

    // Fill in sh2_AsyncEvent_t fields selectively based on the eventId
    if (evt->eventId == SH2_GET_FEATURE_RESP) { // Populate the sensorConfigResp
        // field with the SHTP_EVENT value.
        sh2SensorConfigResp =
            c_to_SensorConfigResp(env, &evt->sh2SensorConfigResp);
        status |= napi_get_null(env, &shtpEvent);
    } else if (evt->eventId == SH2_SHTP_EVENT) {
        status |= napi_get_null(env, &sh2SensorConfigResp);
        status |= napi_create_uint32(env, evt->shtpEvent, &shtpEvent);
    } else { // It's SH2_RESET, neither of shtpEvent or sensorConfig is set
        status |= napi_get_null(env, &shtpEvent);
        status |= napi_get_null(env, &sh2SensorConfigResp);
    }

    status |= napi_create_uint32(env, evt->eventId, &eventId);
    status |= napi_set_named_property(env, obj, "id", eventId);
    status |= napi_set_named_property(env, obj, "shtpEvent", shtpEvent);
    status |= napi_set_named_property(env, obj, "sh2SensorConfigResp",
                                      sh2SensorConfigResp);

    if (status != napi_ok) {
        napi_throw_error(env, ERROR_TRANSLATING_STRUCT_TO_NODE,
                         "couldn't construct AsyncEvent");
        return NULL;
    }
    return obj;
}

int8_t from_SensorConfig_to_c(napi_env env, napi_value value,
                              sh2_SensorConfig_t* result) {
    napi_status status;
    napi_value changeSensitivityEnabled;
    napi_value changeSensitivityRelative;
    napi_value wakeupEnabled;
    napi_value alwaysOnEnabled;
    napi_value sniffEnabled;
    napi_value changeSensitivity;
    napi_value reportInterval_us;
    napi_value batchInterval_us;
    napi_value sensorSpecific;

    memset(result, 0, sizeof(sh2_SensorConfig_t));
    bool has_prop;

    // Get the properties from the JS object

    // changeSensitivityEnabled
    status = napi_has_named_property(env, value, "changeSensitivityEnabled",
                                     &has_prop);
    if (has_prop) {
        status = napi_get_named_property(env, value, "changeSensitivityEnabled",
                                         &changeSensitivityEnabled);
        if (status != napi_ok) { return EXIT_FAILURE; }
        status = napi_get_value_bool(env, changeSensitivityEnabled,
                                     &result->changeSensitivityEnabled);
        if (status != napi_ok) { return EXIT_FAILURE; }
    }

    // changeSensitivityRelative
    status = napi_has_named_property(env, value, "changeSensitivityRelative",
                                     &has_prop);
    if (has_prop) {
        status =
            napi_get_named_property(env, value, "changeSensitivityRelative",
                                    &changeSensitivityRelative);
        if (status != napi_ok) { return EXIT_FAILURE; }
        status = napi_get_value_bool(env, changeSensitivityRelative,
                                     &result->changeSensitivityRelative);
        if (status != napi_ok) { return EXIT_FAILURE; }
    }

    // wakeupEnabled
    status = napi_has_named_property(env, value, "wakeupEnabled", &has_prop);
    if (has_prop) {
        status = napi_get_named_property(env, value, "wakeupEnabled",
                                         &wakeupEnabled);
        if (status != napi_ok) { return EXIT_FAILURE; }
        status =
            napi_get_value_bool(env, wakeupEnabled, &result->wakeupEnabled);
        if (status != napi_ok) { return EXIT_FAILURE; }
    }

    // alwaysOnEnabled
    status = napi_has_named_property(env, value, "alwaysOnEnabled", &has_prop);
    if (has_prop) {
        status = napi_get_named_property(env, value, "alwaysOnEnabled",
                                         &alwaysOnEnabled);
        if (status != napi_ok) { return EXIT_FAILURE; }
        bool tmp;
        status = napi_get_value_bool(env, alwaysOnEnabled, &tmp);
        if (status != napi_ok) { return EXIT_FAILURE; }
        result->alwaysOnEnabled = tmp ? true : false;
    }

    // sniffEnabled
    status = napi_has_named_property(env, value, "sniffEnabled", &has_prop);
    if (has_prop) {
        status =
            napi_get_named_property(env, value, "sniffEnabled", &sniffEnabled);
        if (status != napi_ok) { return EXIT_FAILURE; }
        status = napi_get_value_bool(env, sniffEnabled, &result->sniffEnabled);
        if (status != napi_ok) { return EXIT_FAILURE; }
    }

    // changeSensitivity
    status =
        napi_has_named_property(env, value, "changeSensitivity", &has_prop);
    if (has_prop) {
        status = napi_get_named_property(env, value, "changeSensitivity",
                                         &changeSensitivity);
        if (status != napi_ok) { return EXIT_FAILURE; }
        uint32_t tmp;
        status = napi_get_value_uint32(env, changeSensitivity, &tmp);
        if (status != napi_ok) {
            napi_throw_error(env, ERROR_TRANSLATING_NODE_TO_STRUCT,
                             "changeSensitivity is not a uint32.");
            return EXIT_FAILURE;
        }
        if (tmp > UINT16_MAX) { // Out of range
            return EXIT_FAILURE;
        }
        result->changeSensitivity = (uint16_t)tmp;
    }

    // reportInterval_us
    status =
        napi_has_named_property(env, value, "reportInterval_us", &has_prop);
    if (has_prop) {
        status = napi_get_named_property(env, value, "reportInterval_us",
                                         &reportInterval_us);
        if (status != napi_ok) { // Invalid property
            return EXIT_FAILURE;
        }
        uint32_t tmp;
        status = napi_get_value_uint32(env, reportInterval_us, &tmp);
        if (status != napi_ok) { return EXIT_FAILURE; }
        result->reportInterval_us = tmp;
    }

    // batchInterval_us
    status = napi_has_named_property(env, value, "batchInterval_us", &has_prop);
    if (status != napi_ok) {
        char error[200];
        snprintf(error, 200,
                 "napi_has_named_property(SensorConfig, batchInterval_us) "
                 "returned with napi status of %d",
                 status);
        fprintf(stderr, "%s\n", error);
        return EXIT_FAILURE;
    }
    if (has_prop) {
        status = napi_get_named_property(env, value, "batchInterval_us",
                                         &batchInterval_us);
        if (status != napi_ok) { // Invalid property
            return EXIT_FAILURE;
        }
        uint32_t tmp;
        status = napi_get_value_uint32(env, batchInterval_us, &tmp);
        if (status != napi_ok) { return EXIT_FAILURE; }
        result->batchInterval_us = tmp;
    }

    // sensorSpecific
    status = napi_has_named_property(env, value, "sensorSpecific", &has_prop);
    if (has_prop) {
        status = napi_get_named_property(env, value, "sensorSpecific",
                                         &sensorSpecific);
        if (status != napi_ok) { // Invalid property
            return EXIT_FAILURE;
        }
        size_t sensorSpecificLen;
        void* sensorSpecificPtr;
        status = napi_get_buffer_info(env, sensorSpecific, &sensorSpecificPtr,
                                      &sensorSpecificLen);
        if (status != napi_ok) { return EXIT_FAILURE; }
        if (sensorSpecificLen > 4) { // 4 is the max length of sensorSpecific
            return EXIT_FAILURE;
        }
        memcpy(&result->sensorSpecific, sensorSpecificPtr, sensorSpecificLen);
    }
    return EXIT_SUCCESS;
}
