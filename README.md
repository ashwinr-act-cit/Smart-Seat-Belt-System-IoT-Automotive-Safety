# Smart Seat Belt System: Biometric & IoT Automotive Safety

> **Tagline:** Proactive automotive safety through real-time biometric monitoring, kinematic analysis, and adaptive haptic feedback.

##  The Problem
Traditional seat belts are strictly reactive—they only engage *after* an accident has occurred. They do nothing to prevent crashes caused by driver fatigue, sudden medical emergencies, or drowsiness, which account for a massive percentage of road accidents globally.

##  Our Solution
The Smart Seat Belt System upgrades passive automotive safety into a proactive, intelligent framework. By embedding biometric and kinematic sensors directly into the seat belt mechanism, the system continuously monitors the driver's physiological state and vehicle telemetry to anticipate accidents and alert the driver before a critical failure occurs.

## Core Hardware & Architecture
As an embedded systems developer, my focus was on integrating the hardware layer to ensure zero-latency data processing:

* **Kinematic Crash Anticipation:** Integrated MPU-6050 (Accelerometer & Gyroscope) to detect sudden anomalies like rapid braking, swerving, or skidding.
* **Biometric Telemetry:** Wired physiological sensors to track vital parameters including heart rate, respiration, and Oxygen Saturation (SpO₂).
* **Drowsiness Detection & Haptics:** Built a firmware loop that identifies falling biometric baselines (fatigue) and triggers a vibration motor embedded in the belt to jolt the driver awake.
* **Adaptive Tension Mechanism:** Programmed dynamic adjustments to seat belt tension based on the driver's condition and real-time driving scenarios.

##  Repository Structure
* **`/firmware`**: C/C++ code for the microcontroller handling sensor polling and haptic feedback triggers.
* **`/schematics`**: Circuit diagrams and pinout configurations for the sensor array.
* **`/data_processing`**: IoT scripts for handling real-time biometric telemetry.

##  Technology Stack
* **Microcontrollers:** Arduino / Standard Embedded Boards
* **Sensors:** MPU-6050 (6-DoF IMU), Pulse Oximeter/Heart Rate Sensors
* **Actuators:** DC Vibration Motors
* **Domain:** IoT, ADAS (Advanced Driver Assistance Systems)

##  Future Scope
This prototype lays the foundational hardware framework for future integration with AI-driven predictive safety models and autonomous vehicle central computers.
