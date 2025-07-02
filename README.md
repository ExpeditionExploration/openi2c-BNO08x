# BNO08X
Datasheet:
https://www.ceva-ip.com/wp-content/uploads/2019/10/BNO080_085-Datasheet.pdf


This standalone interface provides I2C driver bindings for
[OpenI2C](https://github.com/ExpeditionExploration/openi2c)


## References

https://cdn-learn.adafruit.com/downloads/pdf/adafruit-9-dof-orientation-imu-fusion-breakout-bno085.pdf
https://cdn.sparkfun.com/assets/4/d/9/3/8/SH-2-Reference-Manual-v1.2.pdf

https://www.ceva-ip.com/wp-content/uploads/SH-2-Reference-Manual.pdf


## About

BNO08x is a 9-axis IMU with accelerometer, magnetometer and gyroscope.

This repo contains [Node.js](https://nodejs.org/en) bindings to Ceva [SH2 Sensorhub driver for MCU application](https://github.com/ceva-dsp/sh2) driver.
This driver is Linux-specific. 

The bindings are written in C, and need to be compiled for them to work. There
are no pre-compiled binaries. Installing by installing OpenI2C should get you
going, but if you want this driver only, see *Building* for how to build the
binary for Node.


## Prerequisites

A C toolchain is needed for building, as well as a working Node.js installation.

I2C bus needs to be enabled for your Linux. In case of Raspbian, using
`sudo raspi-config` will do the trick. In Raspbian the command `sudo apt install build-essential` can be issued to install build prerequisites.


## Building

If, however, you want to build the bindings independently, you can

```bash
npm install
npm run build
```


## Usage

See the [example](./src/example.ts).


## Missing features

Not all of the user mode part of the driver features have been implemented.
Most notably rotary table calibration of the Motion Engine is still missing.


## Tested boards

This has been tested on:
- *Raspberry Pi 4B* using *Node v22.14.0*

