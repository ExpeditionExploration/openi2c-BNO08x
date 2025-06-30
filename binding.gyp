{
    "targets": [{
        "target_name": "bno08x_native",
        "sources": [
            "src/c-src/binding.c",
            "src/c-src/sh2_hal.c",
            "src/c-src/error.c",
            "src/c-src/funcs.c",
            "src/c-src/sensor_report_auxialiry_fns.c",
            "src/c-src/error.c",
            "src/c-src/node_c_type_conversions.c"
        ],
        "include_dirs": [
            "src/c-include"
        ],
        "cflags": [
            "-Wall",    # Enable all warnings
            "-Werror",  # Make them errors
            "-Wno-missing-braces", # Require braces
            "-Wextra",
            "-pedantic",
            "-fPIC"     # Generate position independent code
        ],
        "dependencies": [
            "sh2"
        ]
    }, {
        "target_name": "sh2",
        "type": "static_library",
        "sources": [
            "src/c-src/sh2/sh2.c",
            "src/c-src/sh2/euler.c",
            "src/c-src/sh2/sh2_SensorValue.c",
            "src/c-src/sh2/sh2_util.c",
            "src/c-src/sh2/shtp.c"
        ],
        "include_dirs": [
            "src/c-src/sh2/"
        ],
        "cflags": [
            "-Wall",    # Enable all warnings
            "-Werror",  # Make them errors
            "-Wno-missing-braces", # Require braces
            "-Wextra",
            "-pedantic",
            "-fPIC"     # Generate position independent code
        ]
    }]
}
