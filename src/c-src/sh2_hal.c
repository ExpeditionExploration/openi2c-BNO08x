#include "sh2/sh2_hal.h"
#include "error.h"
#include "sh2/sh2.h"
#include "sh2_hal_supplement.h"
#include <errno.h>
#include <fcntl.h>
#include <linux/i2c-dev.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <time.h>
#include <unistd.h>

static i2c_settings_t CURRENT_I2C_SETTINGS;

// This function initializes communications with the device.  It
// can initialize any GPIO pins and peripheral devices used to
// interface with the sensor hub.
// It should also perform a reset cycle on the sensor hub to
// ensure communications start from a known state.
int open_i2c(sh2_Hal_t* self) { return 0; }

// This function completes communications with the sensor hub.
// It should put the device in reset then de-initialize any
// peripherals or hardware resources that were used.
void close_i2c(sh2_Hal_t* self) {}

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
    char            dev[12]  = {0x00};
    snprintf(dev, 12, "/dev/i2c-%hhd", settings->bus);

    int fd = open(dev, O_RDONLY);
    if (fd < 0) {
        perror(ERROR_OPENING_DEVICE);
        return 0;
    }

    if (ioctl(fd, I2C_SLAVE, settings->addr) < 0) {
        perror(ERROR_ACQUIRING_SLAVE);
        close(fd);
        return 0;
    }

    uint8_t buf[len];
    size_t  total_read = 0;
    ssize_t read_bytes;
    int     zero_read_attempts = 0;
    while (total_read < len) {
        read_bytes = read(fd, buf + total_read, len - total_read);
        if (read_bytes < 0) {
            perror(ERROR_WRITING_TO_SLAVE);
            printf("error code: %s\n", strerror(errno));
            return 0;
        } else if (read_bytes < 1 && ++zero_read_attempts > 3) { // caution
            perror(ERROR_READING_FULL_BUFFER);
            return 0;
        } else {
            total_read += read_bytes;
        }
    }
    memcpy(pBuffer, buf, len);

    close(fd);
    return 1;
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
    i2c_settings_t* settings = &CURRENT_I2C_SETTINGS;
    char            dev[12]  = {0x00};
    snprintf(dev, 12, "/dev/i2c-%hhd", settings->bus);

    int fd = open(dev, O_WRONLY);
    if (fd < 0) {
        perror(ERROR_OPENING_DEVICE);
        return 0;
    }

    if (ioctl(fd, I2C_SLAVE, settings->addr) < 0) {
        perror(ERROR_ACQUIRING_SLAVE);
        close(fd);
        return 0;
    }

    size_t  total_sent = 0;
    ssize_t sent;
    while (total_sent < len) {
        sent = write(fd, pBuffer, len);
        if (sent < 0) {
            perror(ERROR_WRITING_TO_SLAVE);
            printf("error code: %s\n", strerror(errno));
            return 0;
        } else if (sent < 1) {
            printf("Really? Wrote 0 bytes?\n");
            return 0;
        } else {
            total_sent += sent;
        }
    }

    close(fd);
    return 1;
}

/**
 * Return uint32_t which is the time since sytem boot in micro seconds. The
 * value starts from 0 again in case it would exceed the 32-bit range.
 */
uint32_t get_time_us(sh2_Hal_t* self) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_nsec % 4294967295; /* Maximum for uint32_t */
}

void set_i2c_settings(i2c_settings_t* settings) {
    CURRENT_I2C_SETTINGS = *settings;
}

i2c_settings_t get_i2c_settings() { return CURRENT_I2C_SETTINGS; }

sh2_Hal_t make_hal() {
    sh2_Hal_t hal = {.open      = open_i2c,
                     .close     = close_i2c,
                     .read      = read_from_i2c,
                     .write     = write_to_i2c,
                     .getTimeUs = get_time_us};
    return hal;
}
