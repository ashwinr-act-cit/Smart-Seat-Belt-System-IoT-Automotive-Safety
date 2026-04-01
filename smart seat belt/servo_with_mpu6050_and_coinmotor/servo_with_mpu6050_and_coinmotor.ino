#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <ESP32Servo.h>

Adafruit_MPU6050 mpu;  // Create MPU6050 object
Servo myServo;         // Create Servo object for ESP32

// Sensitivity Thresholds (Adjust as Needed)
const float accelThreshold = 13;  // Sudden acceleration (g)
const float gyroThreshold = 50;   // Sudden rotation (degrees/sec)

// Servo Control Variables
unsigned long servoMoveTime = 0;  // Timestamp for servo movement
bool servoActive = false;         // Flag to track servo state

#define BUTTON_PIN 12   // Push button connected to GPIO 15
#define MOTOR_PIN  13   // Motor connected to 3.3V (not a GPIO, just power)

void setup() {
    Serial.begin(115200);
    Wire.begin();
    
    // Initialize MPU6050
    if (!mpu.begin()) {
        Serial.println("MPU6050 connection failed! Check wiring.");
        while (1);
    }
    
    // Configure Accelerometer & Gyroscope
    mpu.setAccelerometerRange(MPU6050_RANGE_2_G);
    mpu.setGyroRange(MPU6050_RANGE_250_DEG);
    mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);

    // Servo Setup for ESP32
    myServo.setPeriodHertz(50);  // Set PWM frequency for servo (50Hz)
    myServo.attach(12);  // Connect servo signal wire to GPIO 13 (Adjust if needed)
    myServo.write(0);     // Start servo at 0 degrees

    Serial.println("System Ready...");

  pinMode(BUTTON_PIN, INPUT);  // Set button as input with pull-down resistor
  pinMode(MOTOR_PIN, OUTPUT);
}

void loop() {
    sensors_event_t accel, gyro, temp;
    mpu.getEvent(&accel, &gyro, &temp);  // Get MPU6050 sensor data

    float accelMagnitude = sqrt(sq(accel.acceleration.x) + sq(accel.acceleration.y) + sq(accel.acceleration.z));
    float gyroMagnitude = sqrt(sq(gyro.gyro.x) + sq(gyro.gyro.y) + sq(gyro.gyro.z)) * 57.2958; // Convert to degrees/sec

    Serial.print("Accel: "); Serial.print(accelMagnitude); Serial.print(" g, ");
    Serial.print("Gyro: "); Serial.print(gyroMagnitude); Serial.println(" deg/sec");

    // Check if movement exceeds threshold
    myServo.write(0);
    if (accelMagnitude > accelThreshold || gyroMagnitude > gyroThreshold) {
        Serial.println("⚠ Sudden Movement Detected! Activating Servo...");
        myServo.write(90);  // Move Servo to 90 degrees
        servoMoveTime = millis();  // Record the time
        servoActive = true;        // Set servo state to active
    }

    // Non-blocking servo reset after 1 second
    if (servoActive && (millis() - servoMoveTime >= 1000)) {
        myServo.write(0);   // Reset Servo position
        servoActive = false; // Set servo state to inactive
    }
    if (digitalRead(BUTTON_PIN) == HIGH) {
    digitalWrite(MOTOR_PIN, HIGH);
   // Serial.println("Button Pressed! Motor should vibrate.");
    // No need to control GPIO, motor gets direct 3.3V power
  } 
  else{
    digitalWrite(MOTOR_PIN, LOW);
  }
  delay(100);  // Small delay to prevent excessive readings
}