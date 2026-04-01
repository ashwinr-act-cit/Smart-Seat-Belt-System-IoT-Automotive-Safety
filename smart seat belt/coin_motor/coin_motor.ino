#define BUTTON_PIN 12   // Push button connected to GPIO 15
#define MOTOR_PIN  13   // Motor connected to 3.3V (not a GPIO, just power)

void setup() {
  pinMode(BUTTON_PIN, INPUT);  // Set button as input with pull-down resistor
  pinMode(MOTOR_PIN, OUTPUT);
}

void loop() {
  if (digitalRead(BUTTON_PIN) == HIGH) {
    digitalWrite(MOTOR_PIN, HIGH);
   // Serial.println("Button Pressed! Motor should vibrate.");
    // No need to control GPIO, motor gets direct 3.3V power
  } 
  else{
    digitalWrite(MOTOR_PIN, LOW);
  }
}