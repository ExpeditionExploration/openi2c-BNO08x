import { EventEmitter } from 'events';
import { bindings, SensorConfig, SensorId } from './index.js';

export interface SensorData {
    type: string;
    values: Record<string, number>;
    time: number;
    delay: number;
}

export const sensorStream = new EventEmitter();

let pollInterval: ReturnType<typeof setInterval>;
const toMs = (us: bigint) => Number(us / 1000n);

function callback(ev: any) {
    const data: SensorData = {
        type: SensorId[ev.reportId],
        values: {},
        time: toMs(ev.timestampMicroseconds),
        delay: ev.delayMicroseconds / 1000,
    };
    switch (ev.reportId) {
        case SensorId.SH2_ACCELEROMETER:
            data.values = { x: +ev.x, y: +ev.y, z: +ev.z };
            break;
        case SensorId.SH2_GRAVITY:
            data.values = { x: +ev.x, y: +ev.y, z: +ev.z };
            break;
        case SensorId.SH2_GYROSCOPE_UNCALIBRATED:
            data.values = { x: +ev.x, y: +ev.y, z: +ev.z };
            break;
        case SensorId.SH2_LINEAR_ACCELERATION:
            data.values = { x: +ev.x, y: +ev.y, z: +ev.z };
            break;
        case SensorId.SH2_MAGNETIC_FIELD_UNCALIBRATED:
            data.values = { x: +ev.x, y: +ev.y, z: +ev.z };
            break;
        case SensorId.SH2_RAW_MAGNETOMETER:
            data.values = { x: +ev.x, y: +ev.y, z: +ev.z };
            break;
        case SensorId.SH2_ROTATION_VECTOR:
            data.values = { x: +ev.i, y: +ev.j, z: +ev.k, w: +ev.real, yaw: +ev.yaw, pitch: +ev.pitch, roll: +ev.roll };
            //console.log(`ROT VECTOR, X: ${ev.i}, Y: ${ev.j}, Z: ${ev.k}, W: ${ev.real} -- Time: ${ev.timestampMicroseconds / 1000n}ms, Delay: ${ev.delayMicroseconds / 1000}ms`);
            //console.log(`ROT VECTOR, Yaw: ${ev.yaw}, Pitch: ${ev.pitch}, Roll: ${ev.roll} -- Time: ${ev.timestampMicroseconds / 1000n}ms, Delay: ${ev.delayMicroseconds / 1000}ms`);
            break;
    }
    sensorStream.emit('data', data);
}

export function startSensor(): void {
    bindings.setI2CConfig(5, 0x4b);
    bindings.open(() => { }, { cookie: {} });
    bindings.setSensorCallback(callback, { cookie: {} });

    const ON: SensorConfig = { alwaysOnEnabled: true, reportInterval_us: 20000 };
    const OFF: SensorConfig = { alwaysOnEnabled: false, reportInterval_us: 0 };

    bindings.setSensorConfig(SensorId.SH2_ACCELEROMETER, OFF);
    bindings.setSensorConfig(SensorId.SH2_GRAVITY, OFF);
    bindings.setSensorConfig(SensorId.SH2_GYROSCOPE_UNCALIBRATED, OFF);
    bindings.setSensorConfig(SensorId.SH2_LINEAR_ACCELERATION, OFF);
    bindings.setSensorConfig(SensorId.SH2_MAGNETIC_FIELD_UNCALIBRATED, OFF);
    bindings.setSensorConfig(SensorId.SH2_RAW_MAGNETOMETER, OFF);
    bindings.setSensorConfig(SensorId.SH2_ROTATION_VECTOR, ON);
    bindings.devOn();

    pollInterval = setInterval(() => {
        bindings.service();
    }, 100);
}

export function stopSensor(): void {
    clearInterval(pollInterval);
    bindings.close();
}