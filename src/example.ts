import { bindings, SensorConfig } from '.'

async function main() {
    // Set bus number and device address
    await bindings.setI2CConfig(1, 0x4b)
    await bindings.open((cookie, ev) => { console.log(ev) }, 'lala')
    await bindings.close()

    // const _: SensorConfig = {
    //     changeSensitivity: 0,
    //     alwaysOnEnabled: true,
    //     batchInterval_us: 10000,
    //     sniffEnabled: true,
    //     reportInterval_us: 1000,
    //     sensorSpecific: 0
    // }


    // await bindings.enableFeature(BNO_REPORT_ACCELEROMETER)
    // await bindings.enableFeature(BNO_REPORT_GYROSCOPE)
    // await bindings.enableFeature(BNO_REPORT_MAGNETOMETER)
    // await bindings.enableFeature(BNO_REPORT_ROTATION_VECTOR)
    // await bindings.enableFeature(BNO_REPORT_GAME_ROTATION_VECTOR)

    // await bno.beginCalibration();
    // setInterval(async ()=>{
    //     const acc = await bno.calibrationStatus();
    //     console.log(acc);
    // }, 500)

    setInterval(async () => {
        //const xyz = await bindings.euler();

        //console.log(`XYZ`, xyz);
        //     const [accelX, accelY, accelZ] = await bno.acceleration();
        //     console.log(`Accel: X: ${accelX.toFixed(6)} Y: ${accelY.toFixed(6)}
        //     Z: ${accelZ.toFixed(6)} m/s^2`);

        //     const [gyroX, gyroY, gyroZ] = await bno.gyro();
        //     console.log(`Gyro: X: ${gyroX.toFixed(6)} Y: ${gyroY.toFixed(6)} Z:
        //     ${gyroZ.toFixed(6)} rads/s`);

        //     const [magX, magY, magZ] = await bno.magnetic();
        //     console.log(`Mag: X: ${magX.toFixed(6)} Y: ${magY.toFixed(6)} Z:
        //     ${magZ.toFixed(6)} m/s^2`);

        //     const [quatI, quatJ, quatK, quatReal] = await bno.quaternion();
        //     console.log(`Quat: I: ${quatI.toFixed(6)} J: ${quatJ.toFixed(6)} K:
        //     ${quatK.toFixed(6)} Real: ${quatReal.toFixed(6)}`);
    }, 1000)
}
main();
