import {bindings} from '..';

const bus = parseInt(process.env.I2C_BUS ?? '1');
const address = parseInt(process.env.I2C_ADDRESS ?? '0x4b');

test('Tests creating and saving settings for bno08x connection', () => {
  expect(() => bindings.setI2CConfig(bus, address)).not.toThrow();
  expect(bindings.getI2CConfig()).toMatchObject({
    bus: bus,
    addr: address,
  })
});
