import { bindings, SensorConfig, SensorId } from '.'

const sleep = (ms: number) => new Promise(resolve => setTimeout(resolve, ms));
const bus = process.env.BNO_BUS ? Number(process.env.BNO_BUS) : 1


async function main(): Promise<void> {
    // Set bus number and device address
    bindings.setI2CConfig(bus, 0x4b)
    bindings.open((ev, cookie) => { console.log(ev) }, { cookie: 'cookie must be an object' })
    bindings.setSensorCallback(
        (ev, cookie) => {
            switch (ev.reportId) {
                case SensorId.SH2_ACCELEROMETER:
                    console.log(`ACCEL, X: ${ev.x}, Y: ${ev.y}, Z: ${ev.z} -- Time: ${ev.timestampMicroseconds / 1000n}ms, Delay: ${ev.delayMicroseconds / 1000}ms`)
                    break
                case SensorId.SH2_GRAVITY:
                    console.log(`GRAV, X: ${ev.x}, Y: ${ev.y}, Z: ${ev.z} -- Time: ${ev.timestampMicroseconds / 1000n}ms, Delay: ${ev.delayMicroseconds / 1000}ms`)
                    break
                case SensorId.SH2_GYROSCOPE_UNCALIBRATED:
                    console.log(`GYRO UNCAL, X: ${ev.x}, Y: ${ev.y}, Z: ${ev.z} -- Time: ${ev.timestampMicroseconds / 1000n}ms, Delay: ${ev.delayMicroseconds / 1000}ms`)
                    break
                case SensorId.SH2_LINEAR_ACCELERATION:
                    console.log(`LIN ACCEL, X: ${ev.x}, Y: ${ev.y}, Z: ${ev.z} -- Time: ${ev.timestampMicroseconds / 1000n}ms, Delay: ${ev.delayMicroseconds / 1000}ms`)
                    break
                case SensorId.SH2_MAGNETIC_FIELD_UNCALIBRATED:
                    console.log(`MAG FIELD UNCAL, X: ${ev.x}, Y: ${ev.y}, Z: ${ev.z} -- Time: ${ev.timestampMicroseconds / 1000n}ms, Delay: ${ev.delayMicroseconds / 1000}ms`)
                    break
                case SensorId.SH2_RAW_MAGNETOMETER:
                    console.log(`RAW MAG, X: ${ev.x}, Y: ${ev.y}, Z: ${ev.z} -- Time: ${ev.timestampMicroseconds / 1000n}ms, Delay: ${ev.delayMicroseconds / 1000}ms`)
                    break
                case SensorId.SH2_ROTATION_VECTOR:
                    console.log(`ROT VECTOR, Yaw: ${ev.yaw}, Pitch: ${ev.pitch}, Roll: ${ev.roll} -- Time: ${ev.timestampMicroseconds / 1000n}ms, Delay: ${ev.delayMicroseconds / 1000}ms`)
                    break
            }
        }, { cookie: 'cookie must be an object' }
    )

    const ON: SensorConfig = {
        alwaysOnEnabled: true,
        reportInterval_us: 2000,
    }
    const OFF: SensorConfig = {
        alwaysOnEnabled: false,
        reportInterval_us: 0,
    }
    bindings.setSensorConfig(SensorId.SH2_ACCELEROMETER, OFF)
    bindings.setSensorConfig(SensorId.SH2_GRAVITY, OFF)
    bindings.setSensorConfig(SensorId.SH2_GYROSCOPE_UNCALIBRATED, ON)
    bindings.setSensorConfig(SensorId.SH2_LINEAR_ACCELERATION, ON)
    bindings.setSensorConfig(SensorId.SH2_MAGNETIC_FIELD_UNCALIBRATED, OFF)
    bindings.setSensorConfig(SensorId.SH2_RAW_MAGNETOMETER, OFF)
    bindings.setSensorConfig(SensorId.SH2_ROTATION_VECTOR, ON)
    bindings.useInterrupts("gpiochip0", 23) // Use GPIO23 for interrupts
    bindings.devOn()

    await sleep(5000)

    // As the last thing print sensorconfig.
    //console.log(`s-config: `, bindings.getSensorConfig(sensorId))

    bindings.close()
}
main().then(() => console.log("done"));
