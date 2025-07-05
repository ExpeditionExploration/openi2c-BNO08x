
export type SensorEvent = {
    timestampMicroseconds: number,
    delayMicroseconds: number,
    len: number,
    reportId: number,
    report: Buffer,
    x?: number,
    y?: number,
    z?: number,
    /**
     * Quaternion i-component
     */
    i?: number,
    /**
     * Quaternion j-component
     */
    j?: number,
    /**
     * Quaternion k-component
     */
    k?: number,
    /**
     * Quaternion real-component
     */
    real?: number,
    pitch?: number,
    yaw?: number,
    roll?: number,
}

export type SensorCallback = (event: SensorEvent, cookie: any) => void;

/**
 * Sensor IDs for BNO08x
 * 
 * Variants with documentation have getter properties added to their
 * `SensorEvent`s. Sensor IDs are also used to designate which sensor to configure
 * in the hub.
 * 
 * Variants without documentation can still be configured and SensorEvents can
 * be received, but no convenience props are provided; Actual values must be
 * fetched from the `Buffer` attached to the event object.
 */
export enum SensorId {
    SH2_RAW_ACCELEROMETER = 0x14,

    /**
     * Acceleration including gravity. Unit is m/s^2.
     * 
     * The following properties are added to `SensorEvent` from this sensor:
     * - `x` *Acceleration along x-axis*
     * - `y` *Acceleration along y-axis*
     * - `z` *Acceleration along z-axis*
     */
    SH2_ACCELEROMETER = 0x01,

    /**
     * Acceleration without gravity. Unit is m/s^2.
     * 
     * The following properties are added to `SensorEvent` from this sensor:
     * - `x` *Acceleration along x-axis*
     * - `y` *Acceleration along y-axis*
     * - `z` *Acceleration along z-axis*
     */
    SH2_LINEAR_ACCELERATION = 0x04,

    /**
     * Gravity components on sensor's coordinate frame. Unit is m/s^2.
     * 
     * The following properties are added to `SensorEvent` from this sensor:
     * - `x` *G x-component*
     * - `y` *G y-component*
     * - `z` *G z-component*
     */
    SH2_GRAVITY = 0x06,
    SH2_RAW_GYROSCOPE = 0x15,

    /**
     * Calibrated gyroscope
     * 
     * The following properties are added to `SensorEvent` from this sensor:
     * - `x` *Angular velocity around x-axis*
     * - `y` *Angular velocity around y-axis*
     * - `z` *Angular velocity around z-axis*
     * 
     * **NOTE:** Calibration functionality is not implemented, and uncalibrated
     * variant should be used.
     */
    SH2_GYROSCOPE_CALIBRATED = 0x02,

    /**
     * Uncalibrated gyroscope
     * 
     * The following properties are added to `SensorEvent` from this sensor:
     * - `x` *Angular velocity around x-axis*
     * - `y` *Angular velocity around y-axis*
     * - `z` *Angular velocity around z-axis*
     */
    SH2_GYROSCOPE_UNCALIBRATED = 0x07,

    /**
     * Magnetic fields on each sensor axis. Units are ADC's.
     * 
     * The following properties are added to `SensorEvent` from this sensor:
     * - `x` *Angular velocity around x-axis*
     * - `y` *Angular velocity around y-axis*
     * - `z` *Angular velocity around z-axis*
     */
    SH2_RAW_MAGNETOMETER = 0x16,

    /**
     * Magnetic fields on each sensor axis. Units are uTesla.
     * See more in the SH-2 Reference Manual.
     * 
     * **NOTE:** Calibration functionality is not implemented, and uncalibrated
     * variant should be used.
     */
    SH2_MAGNETIC_FIELD_CALIBRATED = 0x03,

    /**
     * Magnetic fields on each sensor axis. Units are uTesla.
     * See more in the SH-2 Reference Manual.
     */
    SH2_MAGNETIC_FIELD_UNCALIBRATED = 0x0f,

    /**
     * The rotation vector sensor reports the orientation of the device. The 
     * format of the rotation vector is a unit quaternion. Properties for easy
     * access to yaw, pitch and roll are also provided.
     * 
     * Below are the provided properties for `SensorEvent` having this ID.
     * 
     * # Quaternion
     * - `i` ***i** part* 
     * - `j` ***j** part*
     * - `k` ***k** part*
     * - `r` ***real** part*
     * 
     * # YPR
     * - `yaw` ***Yaw** is the rotation about the vertical axis, turning an object left or right.*
     * - `pitch` ***Pitch** is the rotation about the side-to-side (lateral) axis, tilting an object’s nose up or down.*
     * - `roll` ***Roll** is the rotation about the front-to-back (longitudinal) axis, tipping an object’s wings or sides toward the ground.*
     */
    SH2_ROTATION_VECTOR = 0x05,
    SH2_GAME_ROTATION_VECTOR = 0x08,
    SH2_GEOMAGNETIC_ROTATION_VECTOR = 0x09,
    SH2_SIGNIFICANT_MOTION = 0x12,
    SH2_STABILITY_CLASSIFIER = 0x13,
    SH2_ARVR_STABILIZED_RV = 0x28,
    SH2_ARVR_STABILIZED_GRV = 0x29,
    SH2_GYRO_INTEGRATED_RV = 0x2A,
    SH2_IZRO_MOTION_REQUEST = 0x2B,

    // UPDATE to reflect greatest sensor id
    SH2_MAX_SENSOR_ID = 0x2E,
}

export type SensorConfig = {
    /** Change sensitivity enabled */
    changeSensitivityEnabled?: boolean,

    /** Change sensitivity - true if relative; false if absolute */
    changeSensitivityRelative?: boolean,

    /** Wake-up enabled */
    wakeupEnabled?: boolean,

    /** Always on enabled */
    alwaysOnEnabled: boolean,

    /** Any output from this sensor should be sent to host, but
     * reportInterval_us and sensorSpecific do not influence sensor
     * operation. Not supported by all sensors. */
    sniffEnabled?: boolean,

    /** 16-bit signed fixed point integer representing the value a
     * sensor output must exceed in order to trigger another input
     * report. A setting of 0 causes all reports to be sent.
     */
    changeSensitivity?: number,

    /** Interval in microseconds between asynchronous input reports. */
    reportInterval_us: number,

    /** Reserved field, not used. */
    batchInterval_us?: number,

    /** Meaning is sensor specific */
    sensorSpecific?: Buffer,
}

export enum AsyncEventId {
    RESET,
    SHTP_EVENT,
    GET_FEATURE_RESP,
}

export enum ShtpEvent {
    TX_DISCARD = 0,
    SHORT_FRAGMENT = 1,
    TOO_LARGE_PAYLOADS = 2,
    BAD_RX_CHAN = 3,
    BAD_TX_CHAN = 4,
    BAD_FRAGMENT = 5,
    /**
     * Bad sequence number
     */
    BAD_SN = 6,
    INTERRUPTED_PAYLOAD = 7,
};

export type SensorConfigResponse = {
    sensorId: SensorId;
    sensorConfig: SensorConfig;
}

/// Only shtpEvent or SensorConfigResponse is defined at a time.
export type AsyncEvent = {
    eventId: AsyncEventId;

    /**
     * Either this or `.sh2SensorConfigResp` will be populated, but not both.
     */
    shtpEvent: ShtpEvent | undefined;

    /**
     * Either this or `.shtpEvent`
     */
    sh2SensorConfigResp: SensorConfigResponse | undefined;
}

export type EventCallback = (cookie: Object, event: AsyncEvent) => void;

/**
 * `FrsId` to set or get.
 * 
 * **NOTE:** This enum isn't very useful now, since no calibration functionality
 * is implemented yet. I chose to keep this here regardless, since the functions
 * to set and get FRS *are* implemented. In short, disregard this enum for now.
 */
export enum FrsId {
    /**
     * Static calibration – AGM
     */
    StaticAGMCalibration = 0x7979,
    /**
     * Nominal calibration – AGM
     */
    NominalAGMCalibration = 0x4D4D,
    /**
    Static calibration – SRA
    */
    StaticSRACalibration = 0x8A8A,
    /**
    Nominal calibration - SRA
    */
    NominalSRACalibration = 0x4E4E,
    /**
    Dynamic calibration
    */
    DynamicCalibration = 0x1F1F,
    /**
    MotionEngine power management
    */
    MotionEnginePowerManagement = 0xD3E2,
    /**
    System orientation
    */
    SystemOrientation = 0x2D3E,
    /**
    Primary accelerometer orientation
    */
    PrimaryAccelerometerOrientation = 0x2D41,
    /**
    Screen rotation accelerometer orientation
    */
    ScreenRotationAccelerometerOrientation = 0x2D43,
    /**
    Gyroscope orientation
    */
    GyroscopeOrientation = 0x2D46,
    /**
    Magnetometer orientation
    */
    MagnetometerOrientation = 0x2D4C,
    /**
    AR/VR stabilization – rotation vector
    */
    AR_VR_StabilizationRotationVector = 0x3E2D,
    /**
    AR/VR stabilization – game rotation vector
    */
    AR_VR_StabilizationGameRotationVector = 0x3E2E,
    /**
    Significant Motion detector configuration
    */
    SignificantMotionDetectorConfig = 0xC274,
    /**
    Shake detector configuration
    */
    ShakeDetectorConfig = 0x7D7D,
    /**
    Maximum fusion period
    */
    MaximumFusionPeriod = 0xD7D7,
    /**
    Serial number
    */
    SerialNumber = 0x4B4B,
    /**
    Pickup detector configuration
    */
    PickupDetectorConfig = 0x1B2A,
    /**
    Flip detector configuration
    */
    FlipDetectorConfig = 0xFC94,
    /**
    Stability detector configuration
    */
    StabilityDetectorConfig = 0xED85,
    /**
    Activity Tracker configuration
    */
    ActivityTrackerConfig = 0xED88,
    /**
    Sleep detector configuration
    */
    SleepDetectorConfig = 0xED87,
    /**
    Tilt detector configuration
    */
    TiltDetectorConfig = 0xED89,
    /**
    Pocket detector configuration
    */
    PocketDetectorConfig = 0xEF27,
    /**
    Circle detector configuration
    */
    CircleDetectorConfig = 0xEE51,
    /**
    User record. 64 words or 256 bytes of data of your choice
    */
    UserRecord = 0x74B4,
    /**
    MotionEngine Time Source Selection
    */
    MotionEngineTimeSourceSelect = 0xD403,
    /**
    UART Output Format Selection
    */
    UARTOutputFormat = 0xA1A1,
    /**
    Gyro-Integrated Rotation Vector configuration
    */
    GyroIntegratedRotationVectorConfig = 0xA1A2,
    /**
    Fusion Control Flags
    */
    FusionControlFlags = 0xA1A3,
    /**
    Simple Calibration Configuration
    */
    SimpleCalibrationConfig = 0xA1A4,
    /**
    Nominal Simple Calibration Configuration
    */
    NominalSimpleCalibrationConfig = 0XA1A5,
}

/**
 * @brief BNO08X API
 */
export type BNO08X = {

    setI2CConfig: (bus: number, addr: number) => void,

    /**
     * @brief Open a session with a sensor hub.
     *
     * This function should be called before others in this API.
     *
     * As part of the initialization process, a callback function is registered
     * that will be invoked when the device generates certain events.
     * (See `AsyncEventId`)
     *
     * @param callback is called on events other than sensor events.
     * @param  eventCookie Will be passed to callback. It must be an `Object`,
     * so primitives are a no go. It can be a function, object, array, etc.
     * 
     * @throws `ARGUMENT_ERROR` on invalid arguments.
     * @throws `REF_ERROR` on invalid value to create reference from.
     * @throws `ERROR_INTERACTING_WITH_DRIVER` on failed sh2_open(..).
     */
    open: (callback: EventCallback, eventCookie: Object) => void,

    /**
     * @brief Close a session with a sensor hub.
     *
     * This should be called at the end of a sensor hub session.  
     * The underlying SHTP and HAL instances will be closed.
     *
     */
    close: () => void,

    /**
     * @brief Service the SH2 device, reading any data that is available and
     * dispatching callbacks.
     *
     * This function should be called periodically by the host system to service
     * an open sensor hub.
     *
     * @throws Anything event callbacks might throw.
     * @throws `REF_ERROR` On being unable to fetch value by napi reference.
     * @throws `ERROR_CREATING_NAPI_VALUE` On being unable to fetch JS global
     * object.
     * @throws `THREADING_ERROR` On a callback being invoked from a Node.js
     * thread other than the main thread.
     */
    service: () => void,

    /**
     * @brief Register a function to be called on received sensor events.
     *
     * @param  callback For sensor events.
     * @param  cookie  Will be passed for the callback.
     * 
     * @throws `ARGUMENT_ERROR` On invalid argument.
     * @throws `ERROR_CREATING_NAPI_VALUE` On Out-Of-Memory.
     * @throws `ERROR_TRANSLATING_STRUCT_TO_NODE` If creating a `SensorEvent` fails.
     * @throws `REF_ERROR` On being unable to create a napi reference to cb.
     * @throws `REF_ERROR` On being unable to create a napi reference to cookie.
     * @throws `ERROR_INTERACTING_WITH_DRIVER` On being unable set new cb.
     * 
     */
    setSensorCallback: (callback: SensorCallback, cookie: Object) => void,

    /**
     * @brief Reset the sensor hub.
     *
     * @throws `ERROR_INTERACTING_WITH_DRIVER` on error on resetting the hub.
     */
    devReset: () => void,

    /**
     * @brief Turn sensor hub on.
     *
     * @throws `ERROR_INTERACTING_WITH_DRIVER` on error on enabling the hub.
     */
    devOn: () => void,

    /**
     * @brief Put sensor hub in sleep state.
     *
     * @throws `ERROR_INTERACTING_WITH_DRIVER` on error when trying to sleep.
     */
    devSleep: () => void,

    /**
     * @brief Set sensor configuration.
     * 
     * Each sensor should have it's own configuration. The different sensors for
     * BNO08x are enumerated at `SensorId` enum.
     *
     * @param  sensorId Which sensor to configure.
     * @param  conf `SensorConfig` object.
     * 
     * @throws `ARGUMENT_ERROR` On invalid arguments.
     * @throws `ERROR_CREATING_NAPI_VALUE` On being unable to create cb fn or cookie.
     * @throws `ERROR_INTERACTING_WITH_DRIVER` SH-2 driver returned error code.
     */
    setSensorConfig: (sensorId: SensorId, conf: SensorConfig) => void,

    /**
     * @brief Get an FRS record.
     * 
     * Retrieve stored setting.
     * 
     * **UNTESTED** and since there is no calibration functions yet, it is also
     * not very useful.
     *
     * @param  recordId Which FRS Record to retrieve.
     * 
     * @throws ARGUMENT_ERROR
     * @throws ERROR_CREATING_NAPI_VALUE
     * @throws UNKNOWN_ERROR
     */
    getFrs: (recordId: FrsId) => Buffer,

    /**
     * @brief Set an FRS record
     * 
     * Throws on error.
     * 
     * **UNTESTED** and since there is no calibration functions yet, it is also
     * not very useful.
     *
     * @param  recordId Which FRS Record to set.
     * @param  fsrData Buffer containing data to be written.
     * 
     * @throws ARGUMENT_ERROR
     * @throws UNKNOWN_ERROR
     */
    setFrs: (recordId: FrsId, fsrData: Buffer) => void, // THrows on error
}
