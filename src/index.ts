
const binding = require('bindings')
import {
    type
        SensorEvent, SensorCallback, ProductId, SensorId,
    SensorConfig, SensorMetadata, ErrorRecord, Counts,
    TareBasis, TareAxis, Quaternion, CalibrationStatus,
    InteractiveZROMotionIntent, InteractiveZROMotionRequest,
    AsyncEventId, AsyncEvent, ShtpEvent, SensorConfigResponse,
    EventCallback, Sensors, SensorSet, WheelDatatype, BNO08X
} from "./binding_types"

export const bindings: BNO08X = binding('bno08x_native')
export {
    SensorEvent, SensorCallback, ProductId, SensorId,
    SensorConfig, SensorMetadata, ErrorRecord, Counts,
    TareBasis, TareAxis, Quaternion, CalibrationStatus,
    InteractiveZROMotionIntent, InteractiveZROMotionRequest,
    AsyncEventId, AsyncEvent, ShtpEvent, SensorConfigResponse,
    EventCallback, Sensors, SensorSet, WheelDatatype, BNO08X
}
