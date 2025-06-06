import { bindings } from '..';
import { SensorEvent } from '../binding_types';

const bus = parseInt(process.env.I2C_BUS ?? '1');
const address = parseInt(process.env.I2C_ADDRESS ?? '0x4b');

test('Setting sensor callback doesn\'t throw anything', () => {
    expect(() => bindings.setI2CConfig(bus, address)).not.toThrow();
    expect(
        () => bindings.setSensorCallback(
            (cookie: any, event: SensorEvent) => { }, 'lala'))
        .not.toThrow();
});
