import {bindings} from '..';

const bus = parseInt(process.env.I2C_BUS ?? '1');
const address = parseInt(process.env.I2C_ADDRESS ?? '0x4b');

test('Opening and closing sensor connection', () => {
  expect(() => bindings.setI2CConfig(bus, address)).not.toThrow();
  expect(() => bindings.open((cookie, ev) => {console.log(ev)}, 'lala'))
      .not.toThrow();
  expect(() => bindings.close()).not.toThrow();
});
