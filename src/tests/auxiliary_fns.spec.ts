import { SensorConfig } from '../binding_types';
import { tests } from './test_loader';

test('Converting SensorConfig from JavaScript object to C struct', () => {
  const cfg: SensorConfig = {
    alwaysOnEnabled: true,
    reportInterval_us: 12345,
    batchInterval_us: 12345,
    changeSensitivity: 12345,
    changeSensitivityEnabled: true,
    changeSensitivityRelative: true,
    sniffEnabled: true,
    wakeupEnabled: true,
  }
  expect(() => tests.test_from_SensorConfig_to_c(cfg)).not.toThrow();
});

test('Converting SensorConfig from C struct to JavaScript object', () => {
  const testObject: SensorConfig = tests.test_c_to_SensorConfig()

  expect(testObject.alwaysOnEnabled).toBe(true)
  expect(testObject.batchInterval_us).toBe(12345)
  expect(testObject.changeSensitivity).toBe(12345)
  expect(testObject.changeSensitivityEnabled).toBe(true)
  expect(testObject.changeSensitivityRelative).toBe(true)
  expect(testObject.reportInterval_us).toBe(12345)
  expect(testObject.sniffEnabled).toBe(true)
  expect(testObject.wakeupEnabled).toBe(true)
})
