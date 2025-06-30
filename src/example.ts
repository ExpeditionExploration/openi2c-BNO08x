import { bindings, SensorConfig, SensorId } from '.'
import { BNO_REPORT_ACCELEROMETER } from './constants';


const sleep = (ms: number) => new Promise(resolve => setTimeout(resolve, ms));


async function main(): Promise<void> {
    // Set bus number and device address
    bindings.setI2CConfig(1, 0x4b)
    bindings.open((cookie, ev) => { console.log(ev); }, { foo: 'lala' })
    bindings.setSensorCallback(
        (cookie, ev) => {
            if (ev.reportId == BNO_REPORT_ACCELEROMETER) {
                console.log(`X: ${ev.x}\nY: ${ev.y}\nZ: ${ev.z}`)
                console.log("--------------------")
            }
        }, { foo: 'lala' }
    )

    const cfg: SensorConfig = {
        alwaysOnEnabled: true,
        reportInterval_us: 10000,
    }
    bindings.setSensorConfig(SensorId.SH2_ACCELEROMETER, cfg)
    bindings.devOn()

    for (let i = 0; i < 50; i++) {
        await sleep(200)
        bindings.service()
    }
}
main().then(() => console.log("done"));
