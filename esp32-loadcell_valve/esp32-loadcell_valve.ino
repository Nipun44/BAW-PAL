#include <WiFi.h>
#include <FirebaseESP32.h>
#include "HX711.h"



// HX711 Pins
#define DT 19
#define SCK 18
HX711 scale;

// Update accordingly Calibration
float calibration_factor = 1120.0;

// update Wi-Fi Credentials
#define WIFI_SSID 
#define WIFI_PASSWORD 

// update Firebase Credentials
#define FIREBASE_HOST 
#define FIREBASE_AUTH 

// Firebase objects
FirebaseData firebaseData;
FirebaseConfig config;
FirebaseAuth auth;

// Get timestamp (NTP time setup)
String getFormattedTimestamp() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    Serial.println("Failed to obtain time");
    return "";
  }
  char timestamp[25];
  sprintf(timestamp, "%04d-%02d-%02dT%02d:%02d:%02dZ", 
          timeinfo.tm_year + 1900, timeinfo.tm_mon + 1, timeinfo.tm_mday,
          timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);
  return String(timestamp);
}

void setup() {
  Serial.begin(115200);
  Serial.println("HX711 with Firebase");

  // HX711 init
  scale.begin(DT, SCK);
  Serial.println("Taring... Remove all weight.");
  delay(2000);
  scale.set_scale();
  scale.tare();
  Serial.println("Tare done.");

  // Connect Wi-Fi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(300);
    Serial.print(".");
  }
  Serial.println("\nConnected. IP: " + WiFi.localIP().toString());

  // Setup time (NTP)
  configTime(19800, 0, "pool.ntp.org", "time.nist.gov", "time.google.com");

  // Setup Firebase
  config.host = FIREBASE_HOST;
  config.signer.tokens.legacy_token = FIREBASE_AUTH;
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
}

void loop() {
  if (scale.is_ready()) {
    scale.set_scale(calibration_factor);
    float weight = scale.get_units(5);  // average of 5 readings
    String timestamp = getFormattedTimestamp();

    Serial.print("Weight: ");
    Serial.print(weight, 2);
    Serial.print(" g | ");
    Serial.println("Timestamp: " + timestamp);

    // Create JSON to upload
    FirebaseJson json;
    json.set("weight", weight);
    json.set("timestamp", timestamp);

    // Push to Firebase under /weight_readings
    if (Firebase.pushJSON(firebaseData, "/weight_readings", json)) {
      Serial.println("Weight data pushed to Firebase!");
    } else {
      Serial.println("Firebase Error: " + firebaseData.errorReason());
    }
  } else {
    Serial.println("HX711 not connected.");
  }

  delay(5000); // Delay between readings
}
