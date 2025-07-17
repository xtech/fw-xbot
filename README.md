# xBot Firmware

This repository contains the firmware for the STM32H7 microcontroller on the xBot Core Board, which is part of the xBot robotics development platform.

## Overview

xBot is an educational robotics platform designed for learning ROS2 (Robot Operating System 2). The platform features a 3D-printable robot equipped with:
- Lidar sensor
- Display screen
- Multiple additional sensors
- xBot Mainboard xCore, Lipo Charger, Switchable Power Outputs
- Raspberry Pi CM4 (running ROS2)

This firmware specifically handles the low-level interfaces to control the hardware of the Mainboard.

## System Architecture

The firmware is built on a service-oriented architecture where different functionalities are implemented as services:

- **Emergency Service**: Handles emergency stop and safety features
- **Differential Drive Service**: Controls robot movement
- **IMU Service**: Manages inertial measurement unit
- **Power Service**: Handles power management and charging
- **Input Service**: Manages GPIO and other inputs

## Building

The project uses CMake as its build system.

To build using Docker, run:
```bash
./build-binary-docker.sh
```
The binary will be in the `out` directory.


## License
Check the LICENSE file for details.
