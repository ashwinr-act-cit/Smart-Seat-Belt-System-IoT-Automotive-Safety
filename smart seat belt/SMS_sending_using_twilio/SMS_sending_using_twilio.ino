#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>

const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";

const char* account_sid = "YOUR_ACCOUNT_SID";
const char* auth_token = "YOUR_AUTH_TOKEN";
const char* twilio_number = "+YOUR_TWILIO_NUMBER";
const char* recipient_number = "+RECIPIENT_PHONE_NUMBER";

WiFiClientSecure client;

void setup() {
    Serial.begin(115200);
    WiFi.begin(ssid, password);
    
    Serial.print("Connecting to WiFi");
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println(" Connected!");

    sendSMS("Hello from Arduino!");
}

void sendSMS(String message) {
    if (!client.connect("api.twilio.com", 443)) {
        Serial.println("Connection to Twilio failed");
        return;
    }

    String url = "/2010-04-01/Accounts/" + String(account_sid) + "/Messages.json";
    String postData = "To=" + String(recipient_number) + "&From=" + String(twilio_number) + "&Body=" + message;

    client.print(String("POST ") + url + " HTTP/1.1\r\n" +
                 "Host: api.twilio.com\r\n" +
                 "Authorization: Basic " + base64::encode(String(account_sid) + ":" + auth_token) + "\r\n" +
                 "Content-Type: application/x-www-form-urlencoded\r\n" +
                 "Content-Length: " + postData.length() + "\r\n\r\n" +
                 postData);
    
    Serial.println("SMS Sent!");
}

void loop() {
    // Nothing to do here
}