#include <Wire.h>

void setup() {
  Serial.begin(115200);
  Wire.begin(21, 22); // ESP32 default I2C pins (SDA = 21, SCL = 22)
  
  Serial.println("I2C Scanner");
  for (uint8_t address = 3; address < 120; address++) {
    Wire.beginTransmission(address);
    uint8_t error = Wire.endTransmission();
    
    if (error == 0) {
      Serial.print("Found I2C device at address 0x");
      if (address < 16) {
        Serial.print("0");
      }
      Serial.println(address, HEX);
      delay(500);
    }
  }
}

void loop() {
  // Empty loop, scanner works in setup()
}
