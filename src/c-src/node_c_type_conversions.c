
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
