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

Adafruit_MPU6050 mpu;

// Insert your network credentials
#define WIFI_SSID "WIFI_SSID"
#define WIFI_PASSWORD "WIFI_PASSWORD"

// Insert Firebase project API Key
#define API_KEY "AIzaSyA9l-tZNm2kMVIXrroVUw4GoasQfLEHvhg"

// Insert RTDB URL
#define DATABASE_URL "https://anbsfgjf-default-rtdb.firebaseio.com/"

// Define Firebase Data object
FirebaseData fbdo;

FirebaseAuth auth;
FirebaseConfig config;

unsigned long sendDataPrevMillis = 0;
int count = 0;
bool signupOK = false;

int button = 12;
int motor = 13;
int servo_led = 15;

const float accelThreshold = 13; 
const float gyroThreshold = 50;

float ear = 0.0;

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
  pinMode(servo_led, OUTPUT);
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
        digitalWrite(servo_led, HIGH);
    }
    else{
      digitalWrite(servo_led, LOW);
    }

    if (Firebase.RTDB.setFloat(&fbdo, "sensor-values/acc", accelMagnitude) &&
        Firebase.RTDB.setFloat(&fbdo, "sensor-values/gyro", gyroMagnitude) &&
        Firebase.RTDB.getFloat(&fbdo, "rpi_sensors/ear", ear)) {
        Serial.println("Data uploaded successfully");
    } else {
      Serial.println("Failed to upload data");
      Serial.println("REASON: " + fbdo.errorReason());
    }

    if(ear <= 0.2){
      digitalWrite(motor, HIGH);
    }
    else{
      digitalWrite(motor, LOW);
    }
  }
}