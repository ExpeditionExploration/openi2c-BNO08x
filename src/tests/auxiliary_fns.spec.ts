import { SensorConfig, SensorConfigResponse, SensorEvent, SensorId } from '../binding_types';
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
  expect(() => tests.test_node_to_c_SensorConfig(cfg)).not.toThrow();
});

test('Converting SensorConfig from C struct to JavaScript object', () => {
  const testObject: SensorConfig = tests.test_node_from_c_SensorConfig()

  expect(testObject.alwaysOnEnabled).toBe(true)
  expect(testObject.batchInterval_us).toBe(12345)
  expect(testObject.changeSensitivity).toBe(12345)
  expect(testObject.changeSensitivityEnabled).toBe(true)
  expect(testObject.changeSensitivityRelative).toBe(true)
  expect(testObject.reportInterval_us).toBe(12345)
  expect(testObject.sniffEnabled).toBe(true)
  expect(testObject.wakeupEnabled).toBe(true)
})

test('Converting SensorConfigResp from C struct to JavaScript object', () => {
  const testObject: SensorConfigResponse = tests.test_node_from_c_SensorConfigResp()

  expect(testObject.sensorId).toBe(SensorId.SH2_ACCELEROMETER)
  expect(testObject.sensorConfig.alwaysOnEnabled).toBe(true)
  expect(testObject.sensorConfig.batchInterval_us).toBe(12345)
  expect(testObject.sensorConfig.changeSensitivity).toBe(12345)
  expect(testObject.sensorConfig.changeSensitivityEnabled).toBe(true)
  expect(testObject.sensorConfig.changeSensitivityRelative).toBe(true)
  expect(testObject.sensorConfig.reportInterval_us).toBe(12345)
  expect(testObject.sensorConfig.sniffEnabled).toBe(true)
  expect(testObject.sensorConfig.wakeupEnabled).toBe(true)
})

test('Converting sh2_SensorEvent_t to JavaScript object', () => {
  const testObject: SensorEvent = tests.test_node_from_c_SensorEvent()

  expect(testObject.delayMicroseconds).toBe(12345)
  expect(testObject.length).toBe(2)
  expect(testObject.report).toStrictEqual(Buffer.from([123, 123]))
  expect(testObject.reportId).toBe(1)
  const ts: BigInt = BigInt(12345)
  expect(testObject.timestampMicroseconds).toStrictEqual(ts)
})
