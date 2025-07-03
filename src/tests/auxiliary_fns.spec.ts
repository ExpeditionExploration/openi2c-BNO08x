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
