import { bindings, SensorConfig, SensorId } from '.'

const sleep = (ms: number) => new Promise(resolve => setTimeout(resolve, ms));


async function main(): Promise<void> {
    // Set bus number and device address
    bindings.setI2CConfig(1, 0x4b)
    bindings.open((ev, cookie) => { console.log(ev) }, { foo: 'lala' })
    bindings.setSensorCallback(
        (ev, cookie) => {
            if (ev.reportId == SensorId.SH2_ACCELEROMETER) {
                console.log(`ACCEL X: ${ev.x}, Y: ${ev.y}, Z: ${ev.z} -- Delay: ${ev.delayMicroseconds / 1000}ms`)
            } else if (ev.reportId == SensorId.SH2_LINEAR_ACCELERATION) {
                console.log(`LIN_ACCEL X: ${ev.x}, Y: ${ev.y}, Z: ${ev.z} -- Delay: ${ev.delayMicroseconds / 1000}ms`)
            } else if (ev.reportId == SensorId.SH2_GRAVITY) {
                console.log(`GRAVITY X: ${ev.x}, Y: ${ev.y}, Z: ${ev.z} -- Delay: ${ev.delayMicroseconds / 1000}ms`)
            } else if (ev.reportId == SensorId.SH2_RAW_MAGNETOMETER) {
                console.log(`RAW MAGNETOMETER X: ${ev.x}, Y: ${ev.y}, Z: ${ev.z} -- Delay: ${ev.delayMicroseconds / 1000}ms`)
            } else if (ev.reportId == SensorId.SH2_MAGNETIC_FIELD_UNCALIBRATED) {
                console.log(`MAGNETIC FIELD UC X: ${ev.x}, Y: ${ev.y}, Z: ${ev.z} -- Delay: ${ev.delayMicroseconds / 1000}ms`)
            } else if (ev.reportId == SensorId.SH2_ROTATION_VECTOR) {
                console.log(`ROTATION VECTOR: pitch:${ev.pitch}, yaw:${ev.yaw}, roll:${ev.roll} -- Delay: ${ev.delayMicroseconds / 1000}ms`)
            } else if (ev.reportId == SensorId.SH2_GYROSCOPE_UNCALIBRATED) {
                console.log(`GYRO uncalibrated X: ${ev.x}, Y: ${ev.y}, Z: ${ev.z} -- Delay: ${ev.delayMicroseconds / 1000}ms`)
            }
        }, { foo: 'lala' }
    )

    const cfg_enabled: SensorConfig = {
        alwaysOnEnabled: true,
        reportInterval_us: 5000,
    }
    const cfg_disabled: SensorConfig = {
        alwaysOnEnabled: false,
        reportInterval_us: 0,
    }
    // const sensorId = SensorId.SH2_ACCELEROMETER
    // const sensorId = SensorId.SH2_LINEAR_ACCELERATION
    // const sensorId = SensorId.SH2_GRAVITY
    const sensorId = SensorId.SH2_RAW_MAGNETOMETER // NOT IMPLEMENTED?
    // const sensorId = SensorId.SH2_MAGNETIC_FIELD_UNCALIBRATED
    // const sensorId = SensorId.SH2_ROTATION_VECTOR
    // const sensorId = SensorId.SH2_GYROSCOPE_UNCALIBRATED
    bindings.setSensorConfig(sensorId, cfg_enabled)
    bindings.devOn()

    for (let i = 0; i < 50; i++) {
        await sleep(100)
        bindings.service()
    }

    // As the last thing print sensorconfig.
    console.log(`s-config: `, bindings.getSensorConfig(sensorId))

    bindings.close()
}
main().then(() => console.log("done"));
