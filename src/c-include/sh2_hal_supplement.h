#ifndef SH2_HAL_SUPPLEMENT
#define SH2_HAL_SUPPLEMENT

#include "sh2/sh2_hal.h"

typedef struct {
    uint8_t bus;
    uint8_t addr;
    sh2_Hal_t hal;
    int i2c_fd;
} i2c_settings_t;

void set_i2c_settings(i2c_settings_t *settings);
i2c_settings_t get_i2c_settings(void);

sh2_Hal_t make_hal(void);

#endif
