# integration-of-arduino-and-mpu6050
Objective:
The objective of this experiment is to determine the real-time position (coordinates) of an MPU6050 Inertial Measurement Unit (IMU) sensor by manually interfacing it using the I2C protocol without using any prebuilt libraries. The position is calculated through double integration of the raw accelerometer data received from the sensor.

Methodology:
Sensor Initialization and Communication:
The MPU6050 sensor was manually interfaced with the Arduino Uno using the I2C protocol. Instead of using standard libraries such as Wire.h or MPU6050.h, raw register-level communication was implemented based on the MPU6050 datasheet.

Reading Accelerometer Data:
The raw 16-bit signed integer values from the accelerometer registers (AX, AY, AZ) were read directly. These represent acceleration in the X, Y, and Z directions in terms of LSB units.

Conversion to Acceleration in g:
The raw values were converted into acceleration in terms of gravitational acceleration (g) by dividing by the scale factor (16384.0) assuming a ±2g configuration.

Gravity Compensation:
Since the sensor constantly experiences 1g due to gravity (typically on the Z-axis when upright), gravity was subtracted from the Z-axis readings to isolate linear acceleration due to actual motion.

Motion Detection Threshold:
To avoid drift when the sensor is stationary, the magnitude of the total acceleration vector was calculated. A threshold of 0.03g was used to detect significant movement. When the magnitude fell below this threshold, velocities were reset to zero to prevent false integration due to noise.

Velocity and Position Calculation:
For periods of detected motion, the acceleration values were converted to m/s² by multiplying with 9.81. Then, velocity was updated using the equation v = v + a * dt, and position was updated using x = x + v * dt, where dt is the elapsed time in seconds since the last update.

Real-Time Output:
The resulting X, Y, and Z position values in meters were printed to the serial monitor in real time. These values only changed when motion was detected, and remained constant when the sensor was stationary.

Expected Output:
The expected output of this experiment is a set of real-time position coordinates (X, Y, and Z) displayed in meters on the serial monitor. The values update only when the sensor is in motion and remain constant when it is held still. A typical output format is:
X: 0.02 m   Y: -0.01 m   Z: 0.00 m
This output provides a basic but responsive indication of the direction and magnitude of movement.
