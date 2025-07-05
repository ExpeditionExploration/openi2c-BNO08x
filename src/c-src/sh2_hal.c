#include "sh2/sh2_hal.h"

#include <endian.h>
#include <fcntl.h>
#include <limits.h>
#include <linux/i2c-dev.h>
#include <linux/i2c.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#include "error.h"
#include "sh2/sh2.h"
#include "sh2/sh2_err.h"
#include "sh2_hal_supplement.h"

static i2c_settings_t CURRENT_I2C_SETTINGS;

// This function completes communications with the sensor hub.
// It should put the device in reset then de-initialize any
// peripherals or hardware resources that were used.
void close_i2c(sh2_Hal_t* self) {
    if (sh2_devReset() < 0) {
        fprintf(stderr, "Sensor hub couldn't be reset on close\n");
    }
    if (CURRENT_I2C_SETTINGS.i2c_fd > 0) {
        // Close the I2C device file.
        fprintf(stderr, "Closing I2C device: %d\n",
                CURRENT_I2C_SETTINGS.i2c_fd);
        close(CURRENT_I2C_SETTINGS.i2c_fd);
        CURRENT_I2C_SETTINGS.i2c_fd = -1; // Reset to invalid state
    } else {
        fprintf(stderr, "I2C device is not open.\n");
    }
}

// This function initializes communications with the device.  It
// can initialize any GPIO pins and peripheral devices used to
// interface with the sensor hub.
// It should also perform a reset cycle on the sensor hub to
// ensure communications start from a known state.
int open_i2c(sh2_Hal_t* self) {
    if (CURRENT_I2C_SETTINGS.i2c_fd > 0) {
        // An i2c device is already open.
        fprintf(stderr, "I2C device is already open.\n");
        return 0; // success
    }

    // Open the I2C device file.
    char dev[20];
    snprintf(dev, 20, "/dev/i2c-%d", CURRENT_I2C_SETTINGS.bus);
    if ((CURRENT_I2C_SETTINGS.i2c_fd = open(dev, O_RDWR)) < 0) {
        perror("Failed to open i2c bus");
        return 1;
    }
    fprintf(stdout, "Opened I2C device: %s\n", dev);
    // Set the I2C slave address.
    if (ioctl(CURRENT_I2C_SETTINGS.i2c_fd, I2C_SLAVE,
              CURRENT_I2C_SETTINGS.addr) < 0) {
        perror("Failed to set I2C slave address");
        close(CURRENT_I2C_SETTINGS.i2c_fd);
        CURRENT_I2C_SETTINGS.i2c_fd = -1; // Reset to invalid state
        return 1;
    }
    fprintf(stdout, "Set I2C slave address: 0x%02X\n",
            CURRENT_I2C_SETTINGS.addr);

    uint8_t reset_msg[5] = {
        0x05, 0x00, // Length = 5 bytes total
        0x01,       // Channel = 1 (executable)
        0x00,       // Sequence number = 0
        0x01        // Payload = 1 (“reset”)
    };
    ssize_t n = write(CURRENT_I2C_SETTINGS.i2c_fd, reset_msg, 5);
    if (n != 5) {
        char msg[200];
        snprintf(msg, 200, "Reset msg sent, %ldB was sent instead of 5B\n", n);
        fprintf(stderr, msg);
        return SH2_ERR;
    } else {
        fprintf(stdout, "Reset sent to sensor hub.\n");
        sleep(1); // Give time for the hub to reset.
    }

    return 0;
}

// This function supports reading data from the sensor hub.
// It will be called frequently to service the device.
//
// If the HAL has received a full SHTP transfer, this function
// should load the data into pBuffer, set the timestamp to the
// time the interrupt was detected, and return the non-zero length
// of data in this transfer.
//
// If the HAL has not recevied a full SHTP transfer, this function
// should return 0.
//
// Because this function is called regularly, it can be used to
// perform other housekeeping operations.  (In the case of UART
// interfacing, bytes transmitted are staggered in time and this
// function can be used to keep the transmission flowing.)

int read_from_i2c(sh2_Hal_t* self, uint8_t* pBuffer, unsigned len,
                  uint32_t* t_us) {
    i2c_settings_t* settings = &CURRENT_I2C_SETTINGS;
    static bool is_retry;
    static u_int16_t length;
    if (!is_retry) {
        const ssize_t n = read(settings->i2c_fd, pBuffer, 4); // header
        length = *(u_int16_t*)pBuffer;
        length &= 0x7fff;
        length = le16toh(length);
        if (n < 0) {
            perror("read_from_i2c");
            return 0;
        }
        if (length == 0) { return 0; }
        is_retry = true;
        return 0;
    }
    const ssize_t n = read(settings->i2c_fd, pBuffer, length + 4);
    if (n < 0) {
        perror("read_from_i2c");
        return 0;
    }
    is_retry = false;
    *t_us = self->getTimeUs(self);
    // fprintf(stdout, "read len: %d\n", ret_val);
    return n;
}

// This function supports writing data to the sensor hub.
// It is called each time the application has a block of data to
// transfer to the device.
//
// If the device isn't ready to receive data, this function can
// return 0 without performing the transmit function.
//
// If the transmission can be started, this function needs to
// copy the data from pBuffer and return the number of bytes
// accepted.  It need not block.  The actual transmission of
// the data can continue after this function returns.
int write_to_i2c(sh2_Hal_t* self, uint8_t* pBuffer, unsigned int len) {
    // fprintf(stderr, "write_to_i2c, len=%d\n", len);
    i2c_settings_t* settings = &CURRENT_I2C_SETTINGS;
    ssize_t n = write(settings->i2c_fd, pBuffer, len);
    if (n <= 0)
        return 0;
    else
        return n;
}

/**
 * Return uint32_t which is the time since sytem boot in micro seconds. The
 * value starts from 0 again in case it would exceed the 32-bit range.
 */
uint32_t get_time_us(sh2_Hal_t* self) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    uint64_t t = ts.tv_sec * 1000000 + ts.tv_nsec / 1000;
    return (uint32_t)(t & 0xFFFFFFFF);
}

void set_i2c_settings(i2c_settings_t* settings) {
    CURRENT_I2C_SETTINGS = *settings;
}

i2c_settings_t get_i2c_settings(void) { return CURRENT_I2C_SETTINGS; }

sh2_Hal_t make_hal(void) {
    sh2_Hal_t hal = {.open = open_i2c,
                     .close = close_i2c,
                     .read = read_from_i2c,
                     .write = write_to_i2c,
                     .getTimeUs = get_time_us};
    return hal;
}
