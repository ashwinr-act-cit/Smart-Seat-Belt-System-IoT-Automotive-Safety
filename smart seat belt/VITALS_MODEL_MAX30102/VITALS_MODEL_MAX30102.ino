#include <Wire.h>
#include <MAX30105.h>

MAX30105 particleSensor;

#define debug Serial

void setup() {
  debug.begin(115200);
  while (!debug); // Wait for Serial Monitor (Optional)

  debug.println("MAX30105 Heart Rate & SpO2 Monitor - ESP32");

  // Initialize the MAX30105 sensor
  if (!particleSensor.begin()) {
    debug.println("MAX30105 not found. Check wiring!");
    while (1);
  }

  particleSensor.setup();  // Initialize the sensor
  particleSensor.setPulseAmplitudeRed(0x1F);  // Red LED
  particleSensor.setPulseAmplitudeIR(0x1F);   // IR LED
}

void loop() {
  long red = particleSensor.getRed();
  long ir = particleSensor.getIR();

  // Debugging: Print out IR and Red values to check signal
  // We will now detect the heartbeat manually:
  static uint32_t lastBeatTime = 0;
  static float beatsPerMinute = 0;
  
  // Check if a heartbeat is detected (IR signal should change)
  if (ir > 50000) {  // Example threshold for IR value; adjust as needed
    uint32_t currentTime = millis();
    uint32_t timeDifference = currentTime - lastBeatTime;
    lastBeatTime = currentTime;

    // Calculate BPM (beats per minute)
    beatsPerMinute = 60.0 / (timeDifference / 1000.0);

    debug.print("Heart Rate: ");
    debug.print(beatsPerMinute);
    debug.println(" BPM");
  }

  // Estimate SpO2 based on Red/IR ratio (simple formula)
  if (red > 0 && ir > 0) {
    float ratio = (float)red / (float)ir;
    float spo2 = 110 - (25 * ratio);  // Simplified SpO2 formula
    spo2 = constrain(spo2, 70, 100);  // Limit to realistic range
    debug.print("Oxygen Level: ");
    debug.print(spo2);
    debug.println(" %");
  }

  delay(500);
}