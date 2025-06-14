import { bindings, SensorConfig, SensorId } from '.'


const sleep = (ms: number) => new Promise(resolve => setTimeout(resolve, ms));


async function main() {
    // Set bus number and device address
    bindings.setI2CConfig(1, 0x4b)
    bindings.open((cookie, ev) => { console.log(ev); }, 'lala')

    const cfg: SensorConfig = {
        alwaysOnEnabled: true,
        changeSensitivityEnabled: false,
        wakeupEnabled: true, // Must be true to get reports
        reportInterval_us: 1000,
    }
    bindings.devOn()
    bindings.setSensorConfig(SensorId.SH2_ACCELEROMETER, cfg)
    bindings.setSensorCallback((cookie, ev) => console.log(ev), null);

    for (let i = 0; i < 50; i++) {
        await sleep(200)
        bindings.service()
    }
}
main();
