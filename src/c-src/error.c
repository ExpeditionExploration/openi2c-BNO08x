const char* ERROR_OPENING_DEVICE = "could not open device";
const char* ERROR_ACQUIRING_SLAVE = "could not set fd to slave addr";
const char* ERROR_WRITING_TO_SLAVE = "could not write to slave";
const char* ERROR_READING_FULL_BUFFER =
    "could not read the whole designated message size";
const char* ERROR_INTERACTING_WITH_DRIVER = "some error happened with driver";

const char* ARGUMENT_ERROR = "argument error";
const char* UNKNOWN_ERROR = "unknown error happened with nodejs";
const char* ERROR_TRANSLATING_STRUCT_TO_NODE =
    "could't translate value(s) in C struct as node values";
const char* ERROR_TRANSLATING_NODE_TO_STRUCT =
    "could't translate value(s) in node value to C struct";
const char* ERROR_CREATING_NAPI_VALUE = "couldn't create a napi value";
const char* REF_ERROR = "Unexpected ref count";
const char* SENSOR_REPORT_ERROR = "error constructing report object";

const char* ERROR_EXECUTING_TEST = "bno08x bad test";
const char* ASSERT_ERROR = "bno08x test failed assertion";
