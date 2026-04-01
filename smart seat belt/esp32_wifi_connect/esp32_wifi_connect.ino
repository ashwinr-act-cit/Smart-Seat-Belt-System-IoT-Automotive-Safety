#include <WiFi.h>
#include <WebServer.h>

const char* ssid = "HELIOS";       // Replace with your Wi-Fi SSID
const char* password = "RAVINA**77"; // Replace with your Wi-Fi password

// Define GPIO pin for controlling (GPIO13 in this case)
const int gpioPin = 13;

WebServer server(80); // Create a web server on port 80

// Function to handle the request to set GPIO13 HIGH
void handleSetGPIO() {
  digitalWrite(gpioPin, HIGH);
  server.send(200, "text/plain", "GPIO13 is now HIGH");
}

// Function to handle the request to set GPIO13 LOW
void handleResetGPIO() {
  digitalWrite(gpioPin, LOW);
  server.send(200, "text/plain", "GPIO13 is now LOW");
}

void setup() {
  // Start Serial for debugging
  Serial.begin(115200);

  // Initialize the GPIO pin
  pinMode(gpioPin, OUTPUT);
  digitalWrite(gpioPin, LOW);  // Set GPIO13 to LOW initially

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi!");
  Serial.print("ESP32 IP Address: ");
  Serial.println(WiFi.localIP());

  // Define routes for the server
  server.on("/set_gpio", HTTP_GET, handleSetGPIO);  // Set GPIO13 HIGH
  server.on("/reset_gpio", HTTP_GET, handleResetGPIO);  // Set GPIO13 LOW

  // Start the server
  server.begin();
}

void loop() {
  server.handleClient();  // Listen for incoming client requests
}
