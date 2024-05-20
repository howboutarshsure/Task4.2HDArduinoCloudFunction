#include <SPI.h>
#include <WiFiNINA.h>
#include <ArduinoHttpClient.h>
#include <ArduinoJson.h>

// Replace with your WiFi credentials
#define WIFI_SSID "MINEZ_2G"
#define WIFI_PASSWORD "69699696"

const char* firebaseHost = "trafficlight-3c97f-default-rtdb.asia-southeast1.firebasedatabase.app";
const int LED_BLUE = 4;  // Blue LED pin
const int LED_GREEN = 5; // Green LED pin
const int LED_RED = 6;   // Red LED pin

WiFiSSLClient wifiClient;
HttpClient httpClient(wifiClient, firebaseHost, 443);

void setup() {
  Serial.begin(9600);
  pinMode(LED_BLUE, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_RED, OUTPUT);

  setupWiFi();
}

void loop() {
  if (fetchAndSetLEDStates()) {
    Serial.println("LED states updated successfully.");
  } else {
    Serial.println("Failed to update LED states.");
  }
  delay(500);  // Check every 10 seconds
}

void setupWiFi() {
  Serial.print("Connecting to WiFi...");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println(" connected.");
}

bool fetchAndSetLEDStates() {
  httpClient.get("/leds.json");

  int statusCode = httpClient.responseStatusCode();
  Serial.print("Status code: ");
  Serial.println(statusCode);

  if (statusCode != 200) {
    Serial.print("Failed to fetch data: ");
    Serial.println(statusCode);
    return false;
  }

  String response = httpClient.responseBody();
  Serial.print("Received: ");
  Serial.println(response);

  DynamicJsonDocument doc(256);
  DeserializationError error = deserializeJson(doc, response);
  if (error) {
    Serial.print("JSON deserialize failed: ");
    Serial.println(error.c_str());
    return false;
  }

  digitalWrite(LED_BLUE, doc["blue"] ? HIGH : LOW);  // Turn blue LED on if true, off if false
  digitalWrite(LED_GREEN, doc["green"] ? HIGH : LOW); // Turn green LED on if true, off if false
  digitalWrite(LED_RED, doc["red"] ? HIGH : LOW);     // Turn red LED on if true, off if false

  return true;
}
