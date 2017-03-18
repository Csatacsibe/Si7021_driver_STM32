# Notes

- The driver uses blocking I2C API calls of the STM32 HAL library in all cases.
- The driver uses the Hold Master Mode Si7021 I2C commands for both humidity and temperature measurements.
- The Si7021 returns checksums in some cases but the driver currently does not implement checksum verification.
- Function descriptions and additional notes could be found in the Si7021_driver.h header file.
