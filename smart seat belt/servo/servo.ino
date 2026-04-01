#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <ESP32Servo.h>

// Create MPU6050 object
Adafruit_MPU6050 mpu;

// Create Servo object
Servo myServo;

// Define pin connections
#define SERVO_PIN 13  // Change this if your connection is different
#define BUTTON_PIN 4  // Push button pin

// Variable to track whether servo control is enabled
bool controlActive = false;

void setup() {
  Serial.begin(115200);
  Wire.begin(); // Initialize I2C communication

  // Set the push button pin mode (INPUT_PULLUP uses the internal resistor)
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  // Initialize the MPU6050 sensor
  if (!mpu.begin()) {
    Serial.println("MPU6050 not found! Check your wiring.");
    while (1); // Stop further execution
  }
  Serial.println("MPU6050 connected successfully.");

  // Set up the servo motor
  myServo.attach(SERVO_PIN);
  myServo.write(90); // Default position (neutral)
}

void loop() {
  // Read the push button state (LOW when pressed because of INPUT_PULLUP)
  if (digitalRead(BUTTON_PIN) == LOW) {
    // Toggle control state on each button press
    controlActive = !controlActive;
    
    // Simple debounce delay
    delay(300);
  }
  
  if (controlActive) {
    // Read MPU6050 data
    sensors_event_t a, g, temp;
    mpu.getEvent(&a, &g, &temp);

    // Map the X-axis acceleration from -10g to +10g into a servo angle (0 to 180 degrees)
    // You can adjust these values as needed for your application.
    int servoAngle = map(a.acceleration.x * 100, -1000, 1000, 0, 180);
    servoAngle = constrain(servoAngle, 0, 180);
    
    // Set servo to the mapped angle
    myServo.write(servoAngle);
    
    // Print the acceleration and servo angle for debugging
    Serial.print("Acceleration X: ");
    Serial.print(a.acceleration.x);
    Serial.print(" | Servo Angle: ");
    Serial.println(servoAngle);
  }
  else {
    // When control is disabled, keep the servo at the neutral position
    myServo.write(90);
  }
  
  delay(100); // Short delay for stability
}