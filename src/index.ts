
const binding = require('bindings')
import {
    type SensorEvent, SensorCallback, SensorId, SensorConfig,
    AsyncEventId, AsyncEvent, ShtpEvent, SensorConfigResponse,
    EventCallback, BNO08X
} from "./binding_types"

export const bindings: BNO08X = binding('bno08x_native')
export {
    SensorEvent, SensorCallback, SensorId,
    SensorConfig, AsyncEventId, AsyncEvent,
    ShtpEvent, SensorConfigResponse, EventCallback,
    BNO08X
}
