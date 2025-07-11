import { bindings } from '..';
import { SensorConfig, SensorConfigResponse, SensorEvent, SensorId } from '../binding_types';
import { tests } from './test_loader';

test('Properties x,y and z get added to by the relevant helper', () => {
    const testObject = tests.test_add_xyz_to_sensor_report()

    expect(testObject.x).toBe(0.99609375)
    expect(testObject.y).toBe(0.99609375)
    expect(testObject.z).toBe(0.99609375)
});

test('Properties i,j,k,real and pitch,yaw,roll get added to by the relevant helper',
    () => {
        const testObject = tests.test_add_ypr_to_rotation_vector()

        // Check the fixed point values are correct.
        expect(testObject.i).toBeCloseTo(0.0155639648438, 13)
        expect(testObject.j).toBeCloseTo(0.0155639648438, 13)
        expect(testObject.k).toBeCloseTo(0.0155639648438, 13)
        expect(testObject.real).toBeCloseTo(0.0155639648438, 13)

        // These are calculated by the driver, so being defined is enough.
        expect(testObject.pitch).toBeDefined()
        expect(testObject.yaw).toBeDefined()
        expect(testObject.roll).toBeDefined()


    }
);
