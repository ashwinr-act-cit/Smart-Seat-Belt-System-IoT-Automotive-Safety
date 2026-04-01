#include <Arduino.h>
#if defined(ESP32)
  #include <WiFi.h>
#elif defined(ESP8266)
  #include <ESP8266WiFi.h>
#endif
#include <Firebase_ESP_Client.h>

// Provide the token generation process info.
#include "addons/TokenHelper.h"
// Provide the RTDB payload printing info and other helper functions.
#include "addons/RTDBHelper.h"


#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <ESP32Servo.h>

Adafruit_MPU6050 mpu;
Servo myServo;         // Create Servo object for ESP32

// Insert your network credentials
/*#define WIFI_SSID "Hello Moto"
#define WIFI_PASSWORD "THA81838"
*/

#define WIFI_SSID "ASHWIN-2007 5310"
#define WIFI_PASSWORD "ashwin07"

// Insert Firebase project API Key
#define API_KEY "A##################################"

// Insert RTDB URL
#define DATABASE_URL "https://anbsfgjf-default-rtdb.firebaseio.com/"

// Define Firebase Data object
FirebaseData fbdo;

FirebaseAuth auth;
FirebaseConfig config;

unsigned long sendDataPrevMillis = 0;
int count = 0;
bool signupOK = false;

int motor = 13;
//int servo_led = 15;

const float accelThreshold = 12; 
const float gyroThreshold = 60;

float ear = 0.0;

// Servo Control Variables
unsigned long servoMoveTime = 0;  // Timestamp for servo movement
bool servoActive = false;         // Flag to track servo state

void setup() {
  Serial.begin(115200);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  /* Assign the api key (required) */
  config.api_key = API_KEY;

  /* Assign the RTDB URL (required) */
  config.database_url = DATABASE_URL;

  /* Sign up */
  if (Firebase.signUp(&config, &auth, "", "")) {
    Serial.println("ok");
    signupOK = true;
  } else {
    Serial.printf("%s\n", config.signer.signupError.message.c_str());
  }

  /* Assign the callback function for the long running token generation task */
  config.token_status_callback = tokenStatusCallback; // see addons/TokenHelper.h

  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);

  pinMode(motor, OUTPUT);

  // Initialize MPU6050
  if (!mpu.begin()) {
      Serial.println("MPU6050 connection failed! Check wiring.");
      while (1);
  }
    
  // Configure Accelerometer & Gyroscope
  mpu.setAccelerometerRange(MPU6050_RANGE_2_G);
  mpu.setGyroRange(MPU6050_RANGE_250_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);
  myServo.setPeriodHertz(50);  // Set PWM frequency for servo (50Hz)
  myServo.attach(15);  // Connect servo signal wire to GPIO 13 (Adjust if needed)
  myServo.write(0);     // Start servo at 0 degrees
  //pinMode(servo_led, OUTPUT);
}

void loop() {
  if (Firebase.ready() && signupOK && (millis() - sendDataPrevMillis > 100 || sendDataPrevMillis == 0)) {
    sendDataPrevMillis = millis();

    sensors_event_t accel, gyro, temp;
    mpu.getEvent(&accel, &gyro, &temp);  // Get MPU6050 sensor data

    float accelMagnitude = sqrt(sq(accel.acceleration.x) + sq(accel.acceleration.y) + sq(accel.acceleration.z));
    float gyroMagnitude = sqrt(sq(gyro.gyro.x) + sq(gyro.gyro.y) + sq(gyro.gyro.z)) * 57.2958; // Convert to degrees/sec

    Serial.print("Accel: "); Serial.print(accelMagnitude); Serial.print(" g, ");
    Serial.print("Gyro: "); Serial.print(gyroMagnitude); Serial.println(" deg/sec");


    if (accelMagnitude > accelThreshold || gyroMagnitude > gyroThreshold) {
        Serial.println("⚠ Sudden Movement Detected! Activating Servo...");
        myServo.write(180);  // Move Servo to 90 degrees
        servoMoveTime = millis();  // Record the time
        servoActive = true;        // Set servo state to active
    }

    // Non-blocking servo reset after 1 second
    if (servoActive && (millis() - servoMoveTime >= 3000)) {
        myServo.write(0);   // Reset Servo position
        servoActive = false; // Set servo state to inactive
    }

    if (Firebase.RTDB.setFloat(&fbdo, "sensor-values/acc", accelMagnitude) &&
        Firebase.RTDB.setFloat(&fbdo, "sensor-values/gyro", gyroMagnitude) &&
        Firebase.RTDB.getFloat(&fbdo, "rpi_sensors/ear", &ear)) {
        Serial.println("Data uploaded successfully");
    } else {
      Serial.println("Failed to upload data");
      Serial.println("REASON: " + fbdo.errorReason());
    }

    if(ear <= 0.2 && ear !=0.0){
      digitalWrite(motor, HIGH);
    }
    else{
      digitalWrite(motor, LOW);
    }
  }
}
