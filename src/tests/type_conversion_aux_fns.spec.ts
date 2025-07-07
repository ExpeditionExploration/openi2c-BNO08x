import { bindings } from '..';
import { SensorConfig, SensorConfigResponse, SensorEvent, SensorId } from '../binding_types';
import { tests } from './test_loader';

test('Properties x,y and z get added to by the relevant helper', () => {
    const testObject = tests.test_add_properties_to_acceleration_report()

    expect(testObject.x).toBe(0.99609375)
    expect(testObject.y).toBe(0.99609375)
    expect(testObject.z).toBe(0.99609375)
});
