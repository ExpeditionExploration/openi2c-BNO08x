
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

export type SensorCallback = (cookie: any, event: SensorEvent) => void;

export type ProductId = {
    resetCause: number,
    swVersionMajor: number,
    swVersionMinor: number,
    swPartNumber: number,
    swBuildNumber: number,
    swVersionPatch: number,
    reserved0: number,
    reserved1: number,
}

export enum SensorId {
    SH2_RAW_ACCELEROMETER = 0x14,
    /**
     * Acceleration including gravity. Unit is m/s^2.
     */
    SH2_ACCELEROMETER = 0x01,
    /**
     * Acceleration without gravity. Unit is m/s^2.
     */
    SH2_LINEAR_ACCELERATION = 0x04,
    /**
     * Gravity components on sensor's coordinate frame. Unit is m/s^2.
     */
    SH2_GRAVITY = 0x06,
    SH2_RAW_GYROSCOPE = 0x15,
    SH2_GYROSCOPE_CALIBRATED = 0x02,
    SH2_GYROSCOPE_UNCALIBRATED = 0x07,
    /**
     * Magnetic fields on each sensor axis. Units are ADC's.
     */
    SH2_RAW_MAGNETOMETER = 0x16,
    /**
     * Magnetic fields on each sensor axis. Units are uTesla.
     * See more in the SH-2 Reference Manual.
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
     */
    SH2_ROTATION_VECTOR = 0x05,
    SH2_GAME_ROTATION_VECTOR = 0x08,
    SH2_GEOMAGNETIC_ROTATION_VECTOR = 0x09,
    SH2_PRESSURE = 0x0a,
    SH2_AMBIENT_LIGHT = 0x0b,
    SH2_HUMIDITY = 0x0c,
    SH2_PROXIMITY = 0x0d,
    SH2_TEMPERATURE = 0x0e,
    SH2_RESERVED = 0x17,
    SH2_TAP_DETECTOR = 0x10,
    SH2_STEP_DETECTOR = 0x18,
    SH2_STEP_COUNTER = 0x11,
    SH2_SIGNIFICANT_MOTION = 0x12,
    SH2_STABILITY_CLASSIFIER = 0x13,
    SH2_SHAKE_DETECTOR = 0x19,
    SH2_FLIP_DETECTOR = 0x1a,
    SH2_PICKUP_DETECTOR = 0x1b,
    SH2_STABILITY_DETECTOR = 0x1c,
    SH2_PERSONAL_ACTIVITY_CLASSIFIER = 0x1e,
    SH2_SLEEP_DETECTOR = 0x1f,
    SH2_TILT_DETECTOR = 0x20,
    SH2_POCKET_DETECTOR = 0x21,
    SH2_CIRCLE_DETECTOR = 0x22,
    SH2_HEART_RATE_MONITOR = 0x23,
    SH2_ARVR_STABILIZED_RV = 0x28,
    SH2_ARVR_STABILIZED_GRV = 0x29,
    SH2_GYRO_INTEGRATED_RV = 0x2A,
    SH2_IZRO_MOTION_REQUEST = 0x2B,
    SH2_RAW_OPTICAL_FLOW = 0x2C,
    SH2_DEAD_RECKONING_POSE = 0x2D,
    SH2_WHEEL_ENCODER = 0x2E,

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

export type SensorMetadata = {
    meVersion: number,   /**< @brief Motion Engine Version */
    mhVersion: number,  /**< @brief Motion Hub Version */
    shVersion: number,  /**< @brief SensorHub Version */
    range: number,  /**< @brief Same units as sensor reports */
    resolution: number,  /**< @brief Same units as sensor reports */
    revision: number,  /**< @brief Metadata record format revision */
    power_mA: number,    /**< @brief [mA] Fixed point 16Q10 format */
    minPeriod_uS: number,  /**< @brief [uS] */
    maxPeriod_uS: number,  /**< @brief [uS] */
    fifoReserved: number,  /**< @brief (Unused) */
    fifoMax: number,  /**< @brief (Unused) */
    batchBufferBytes: number,  /**< @brief (Unused) */
    qPoint1: number,     /**< @brief q point for sensor values */
    qPoint2: number,     /**< @brief q point for accuracy or bias fields */
    qPoint3: number,     /**< @brief q point for sensor data change sensitivity */
    vendorIdLen: number, /**< @brief [bytes] */
    vendorId: string;  /**< @brief Vendor name and part number */
    sensorSpecificLen: number,  /**< @brief [bytes] */
    sensorSpecific: Buffer;  /**< @brief See SH-2 Reference Manual */
}

export type ErrorRecord = {
    severity: number,   /**< @brief Error severity, 0: most severe. */
    sequence: number,   /**< @brief Sequence number (by severity) */
    source: number,     /**< @brief 1-MotionEngine, 2-MotionHub, 3-SensorHub, 4-Chip  */
    error: number,      /**< @brief See SH-2 Reference Manual */
    module: number,     /**< @brief See SH-2 Reference Manual */
    code: number,       /**< @brief See SH-2 Reference Manual */
}

export type Counts = {
    offered: number,   /**< @brief [events] */
    accepted: number,  /**< @brief [events] */
    on: number,        /**< @brief [events] */
    attempted: number, /**< @brief [events] */
}

export type TareBasis = {
    SH2_TARE_BASIS_ROTATION_VECTOR: number,             /**< @brief Use Rotation Vector */
    SH2_TARE_BASIS_GAMING_ROTATION_VECTOR: number,      /**< @brief Use Game Rotation Vector */
    SH2_TARE_BASIS_GEOMAGNETIC_ROTATION_VECTOR: number, /**< @brief Use Geomagnetic R.V. */
}

export type TareAxis = {
    SH2_TARE_X: number,  /**< @brief sh2_tareNow() axes bit field */
    SH2_TARE_Y: number,  /**< @brief sh2_tareNow() axes bit field */
    SH2_TARE_Z: number,  /**< @brief sh2_tareNow() axes bit field */
    SH2_TARE_CONTROL_VECTOR_X: number,         /**< @brief Use X axis of source and frame to perform tare */
    SH2_TARE_CONTROL_VECTOR_Y: number,         /**< @brief Use Y axis of source and frame to perform tare */
    SH2_TARE_CONTROL_VECTOR_Z: number,         /**< @brief Use Z axis of source and frame to perform tare */
    SH2_TARE_CONTROL_SEQUENCE_DEFAULT: number, /**< @brief Tare "typical" toration for source/axis combination */
    SH2_TARE_CONTROL_SEQUENCE_PRE: number,     /**< @brief Apply to pre-rotation (tare world to device) */
    SH2_TARE_CONTROL_SEQUENCE_POST: number,    /**< @brief Apply to post-rotation (tare device to world) */
}

export type Quaternion = {
    x: number,
    y: number,
    z: number,
    w: number,
};

export enum OscillatorType {
    SH2_OSC_INTERNAL = 0,
    SH2_OSC_EXT_CRYSTAL = 1,
    SH2_OSC_EXT_CLOCK = 2,
};

export enum CalibrationStatus {
    SH2_CAL_SUCCESS = 0,
    SH2_CAL_NO_ZRO,
    SH2_CAL_NO_STATIONARY_DETECTION,
    SH2_CAL_ROTATION_OUTSIDE_SPEC,
    SH2_CAL_ZRO_OUTSIDE_SPEC,
    SH2_CAL_ZGO_OUTSIDE_SPEC,
    SH2_CAL_GYRO_GAIN_OUTSIDE_SPEC,
    SH2_CAL_GYRO_PERIOD_OUTSIDE_SPEC,
    SH2_CAL_GYRO_DROPS_OUTSIDE_SPEC,
};

export enum InteractiveZROMotionIntent {
    SH2_IZRO_MI_UNKNOWN = 0,
    SH2_IZRO_MI_STATIONARY_NO_VIBRATION,
    SH2_IZRO_MI_STATIONARY_WITH_VIBRATION,
    SH2_IZRO_MI_IN_MOTION,
    SH2_IZRO_MI_ACCELERATING,
};

export enum InteractiveZROMotionRequest {
    NO_REQUEST = 0,
    STAY_STATIONARY,
    STATIONARY_NON_URGENT,
    STATIONARY_URGENT,
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
    BAD_SN = 6,
    INTERRUPTED_PAYLOAD = 7,
};

export type SensorConfigResponse = {
    sensorId: SensorId;
    sensorConfig: SensorConfig;
}

/// Only shtpEvent or SensorConfigResponse is defined at a time.
export type AsyncEvent = {
    eventId: number;
    shtpEvent: ShtpEvent | undefined;
    SensorConfigResponse: SensorConfigResponse | undefined;
}

export type EventCallback = (cookie: any, event: AsyncEvent) => void;

export enum Sensors {
    CAL_ACCEL,
    CAL_GYRO,
    CAL_MAG,
    CAL_PLANAR,
    CAL_ON_TABLE,
    CAL_ZERO_GYRO_CONTROL_MASK,
    CAL_ZERO_GYRO_CONTROL_ON_TABLE_DETECT,
    CAL_ZERO_GYRO_CONTROL_NEVER,
    CAL_ZERO_GYRO_CONTROL_ON_TABLE_CLASS,
    CAL_ZERO_GYRO_CONTROL_ON_TABLE_CLASS_OR_LONG_TERM_STABLE
}
export type SensorSet = Sensors;

export enum WheelDatatype {
    Position = 0,
    Velocity = 1,
}

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
    getI2CConfig: () => { bus: number, addr: number },

    /**
     * @brief Open a session with a sensor hub.
     *
     * This function should be called before others in this API.
     * An instance of an SH2 HAL should be passed in.
     * This call will result in the open() function of the HAL being called.
     *
     * As part of the initialization process, a callback function is registered that will
     * be invoked when the device generates certain events.  (See sh2_AsyncEventId)
     *
     * @param pHal Pointer to an SH2 HAL instance, provided by the target system.
     * @param  eventCallback Will be called when events, such as reset complete, occur.
     * @param  eventCookie Will be passed to eventCallback.
     * @return SH2_OK (0), on success.  Negative value from sh2_err.h on error.
     */
    open: (callback: EventCallback, eventCookie: any) => void,

    /**
     * @brief Close a session with a sensor hub.
     *
     * This should be called at the end of a sensor hub session.  
     * The underlying SHTP and HAL instances will be closed.
     *
     */
    close: () => void,

    /**
     * @brief Service the SH2 device, reading any data that is available and dispatching callbacks.
     *
     * This function should be called periodically by the host system to service an open sensor hub.
     *
     */
    service: () => void,

    /**
     * @brief Register a function to receive sensor events.
     *
     * @param  callback A function that will be called each time a sensor event is received.
     * @param  cookie  A value that will be passed to the sensor callback function.
     * @return SH2_OK (0), on success.  Negative value from sh2_err.h on error.
     */
    setSensorCallback: (callback: SensorCallback, cookie: any) => void,  // Throws on error

    /**
     * @brief Reset the sensor hub device by sending RESET (1) command on "device" channel.
     *
     * @return SH2_OK (0), on success.  Negative value from sh2_err.h on error.
     */
    devReset: () => void,

    /**
     * @brief Turn sensor hub on by sending ON (2) command on "device" channel.
     *
     * @return SH2_OK (0), on success.  Negative value from sh2_err.h on error.
     */
    devOn: () => void,

    /**
     * @brief Put sensor hub in sleep state by sending SLEEP (3) command on "device" channel.
     *
     * @return SH2_OK (0), on success.  Negative value from sh2_err.h on error.
     */
    devSleep: () => void,


    /**
     * @brief Get sensor configuration.
     *
     * @param  sensorId Which sensor to query.
     * @param  config SensorConfig structure to store results.
     * @return SH2_OK (0), on success.  Negative value from sh2_err.h on error.
     */
    getSensorConfig: (sensorId: SensorId) => SensorConfig,

    /**
     * @brief Set sensor configuration. (e.g enable a sensor at a particular rate.)
     *
     * @param  sensorId Which sensor to configure.
     * @param  pConfig Pointer to structure holding sensor configuration.
     * @return SH2_OK (0), on success.  Negative value from sh2_err.h on error.
     */
    setSensorConfig: (sensorId: SensorId, conf: SensorConfig) => void, // Throws on error

    /**
     * @brief Get an FRS record.
     * 
     * Retrieve stored setting.
     *
     * @param  recordId Which FRS Record to retrieve.
     */
    getFrs: (recordId: FrsId) => Buffer,

    /**
     * @brief Set an FRS record
     * 
     * If it doesn't throw anything, writing the designated setting was success.
     *
     * @param  recordId Which FRS Record to set.
     * @param  fsrData Buffer containing data to be written.
     * 
     * @throws ARGUMENT_ERROR
     * @throws UNKNOWN_ERROR
     */
    setFrs: (recordId: FrsId, fsrData: Buffer) => void, // THrows on error
}
