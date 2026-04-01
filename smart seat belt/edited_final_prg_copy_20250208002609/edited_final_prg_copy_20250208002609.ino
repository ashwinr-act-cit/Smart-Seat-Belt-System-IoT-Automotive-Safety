#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <Firebase_ESP_Client.h>
#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <ESP32Servo.h>

// Firebase Credentials
#define WIFI_SSID "HELIOS"
#define WIFI_PASSWORD "RAVINA**77"
#define API_KEY "AIzaSyA9l-tZNm2kMVIXrroVUw4GoasQfLEHvhg"
#define DATABASE_URL "https://anbsfgjf-default-rtdb.firebaseio.com/"

// Firebase Data object
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

// MPU6050 Setup
Adafruit_MPU6050 mpu;
Servo myServo;  // Servo object for ESP32

// Motor pin definition
const int gpioPin = 13;
const int motor = 12; // Assuming motor control pin 

// Sensor thresholds for detecting movement
const float accelThreshold = 11.0;
const float gyroThreshold = 10.0;
float ear = 0.0;

// Servo control variables
unsigned long servoMoveTime = 0;
bool servoActive = false;

// WebServer Setup
WebServer server(80); // Create a web server on port 80

void setup() {
  // Serial Debugging
  Serial.begin(115200);

  // Connect to Wi-Fi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(300);
    Serial.print(".");
  }
  Serial.println("\nConnected to Wi-Fi");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  // Firebase Configuration
  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;

  // Sign up with Firebase
  if (Firebase.signUp(&config, &auth, "", "")) {
    Serial.println("Firebase sign-up successful");
  } else {
    Serial.println("Firebase sign-up failed");
    Serial.printf("Error: %s\n", config.signer.signupError.message.c_str());
  }

  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);

  // Initialize MPU6050 sensor
  if (!mpu.begin()) {
    Serial.println("MPU6050 connection failed! Check wiring.");
    while (1);  // Halt if the MPU6050 is not connected
  }

  // Setup Accelerometer and Gyroscope ranges
  mpu.setAccelerometerRange(MPU6050_RANGE_2_G);
  mpu.setGyroRange(MPU6050_RANGE_250_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);

  // Servo setup
  myServo.setPeriodHertz(50);  // Set PWM frequency for servo (50Hz)
  myServo.attach(15);  // Attach servo to GPIO 15 (or change pin as needed)
  myServo.write(0);  // Initialize servo position at 0 degrees

  // Pin Modes
  pinMode(gpioPin, OUTPUT); // Set GPIO pin 13 as output
  pinMode(motor, OUTPUT);   // Set motor control pin as output
  digitalWrite(gpioPin, LOW);  // Set GPIO13 initially to LOW
  digitalWrite(motor, LOW);    // Set motor initially to LOW

  // Web Server Routes
  server.on("/set_gpio", HTTP_GET, handleSetGPIO);   // Set GPIO13 HIGH
  server.on("/reset_gpio", HTTP_GET, handleResetGPIO); // Set GPIO13 LOW
  server.begin(); // Start the web server
}

void handleSetGPIO() {
  digitalWrite(gpioPin, HIGH);
  server.send(200, "text/plain", "GPIO13 is now HIGH");
}

void handleResetGPIO() {
  digitalWrite(gpioPin, LOW);
  server.send(200, "text/plain", "GPIO13 is now LOW");
}

void loop() {
  server.handleClient();  // Handle HTTP client requests

  // Firebase operations
  if (Firebase.ready()) {
    sensors_event_t accel, gyro, temp;
    mpu.getEvent(&accel, &gyro, &temp);  // Get sensor data

    float accelMagnitude = sqrt(sq(accel.acceleration.x) + sq(accel.acceleration.y) + sq(accel.acceleration.z));
    float gyroMagnitude = sqrt(sq(gyro.gyro.x) + sq(gyro.gyro.y) + sq(gyro.gyro.z)) * 57.2958;  // Convert to degrees/sec

    // Display sensor data
    Serial.print("Accel: "); Serial.print(accelMagnitude); Serial.print(" g, ");
    Serial.print("Gyro: "); Serial.print(gyroMagnitude); Serial.println(" deg/sec");

    // Detect movement
    if (accelMagnitude > accelThreshold || gyroMagnitude > gyroThreshold) {
      Serial.println("⚠ Sudden Movement Detected! Activating Servo...");
      myServo.write(180);  // Move Servo to 180 degrees
      servoMoveTime = millis();
      servoActive = true;
    }

    // Reset servo after 2 seconds
    if (servoActive && (millis() - servoMoveTime >= 2000)) {
      myServo.write(0);  // Reset Servo position to 0 degrees
      servoActive = false;
    }

    // Upload sensor data to Firebase
    if (Firebase.RTDB.setFloat(&fbdo, "sensor-values/acc", accelMagnitude) &&
        Firebase.RTDB.setFloat(&fbdo, "sensor-values/gyro", gyroMagnitude) &&
        Firebase.RTDB.getFloat(&fbdo, "rpi_sensors/ear", &ear)) {
      Serial.println("Data uploaded successfully");
    } else {
      Serial.println("Failed to upload data");
      Serial.println("REASON: " + fbdo.errorReason());
    }

    // Control motor based on ear value from Firebase
    if (ear <= 0.2 && ear != 0.0) {
      digitalWrite(motor, HIGH);  // Turn on motor
    } else {
      digitalWrite(motor, LOW);   // Turn off motor
    }
  }
}
