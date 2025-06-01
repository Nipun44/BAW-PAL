#include <WiFi.h>
#include <FirebaseESP32.h>
#include "DHT.h"

// update Wi-Fi Credentials
#define WIFI_SSID 
#define WIFI_PASSWORD 

// update Firebase Config
#define FIREBASE_HOST 
#define FIREBASE_AUTH 


// DHT11 Config
#define DHTPIN 13
#define DHTTYPE DHT11

// HC-SR04 Ultrasonic Config
#define TRIG_PIN 12
#define ECHO_PIN 14


// Firebase objects
FirebaseData firebaseData;
FirebaseConfig config;
FirebaseAuth auth;

// Initialize DHT sensor
DHT dht(DHTPIN, DHTTYPE);

// Get formatted timestamp
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

// Setup function
void setup() {
  Serial.begin(115200);
  
  // Initialize DHT sensor
  dht.begin();
  
  // Initialize Ultrasonic sensor pins
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  // Connect to Wi-Fi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());

  // Initialize system time using multiple NTP servers
  configTime(19800, 0, "pool.ntp.org", "time.nist.gov", "time.google.com"); // Sri Lanka UTC+5:30

  // Initialize Firebase
  config.host = FIREBASE_HOST;
  config.signer.tokens.legacy_token = FIREBASE_AUTH;
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
}

// Function to read distance from ultrasonic sensor
float readDistance() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  
  long duration = pulseIn(ECHO_PIN, HIGH);
  return duration * 0.0343 / 2; // Convert to cm
}

// Loop function
void loop() {
  // Get current timestamp
  String timestamp = getFormattedTimestamp();
  
  // Read DHT11 sensor
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();
  
  // Check if DHT readings failed
  if (isnan(humidity) || isnan(temperature)) {
    Serial.println("Failed to read from DHT sensor!");
    humidity = 0;
    temperature = 0;
  } else {
    Serial.print("Humidity: "); Serial.print(humidity); Serial.print(" %\t");
    Serial.print("Temperature: "); Serial.print(temperature); Serial.println(" °C");
  }

  // Read ultrasonic distance
  float distance = readDistance();
  Serial.print("Distance: "); Serial.print(distance); Serial.println(" cm");
  Serial.print("Timestamp: "); Serial.println(timestamp);
  
  // Create a JSON object with all sensor data
  FirebaseJson json;
  json.set("temperature", temperature);
  json.set("humidity", humidity);

  json.set("distance", distance);
  json.set("timestamp", timestamp);

  // Send data to Firebase
  if (Firebase.pushJSON(firebaseData, "/sensor_readings", json)) {
    Serial.println("Data pushed to Firebase!");
    Serial.print("Path: ");
    Serial.println(firebaseData.dataPath());
  } else {
    Serial.println("Error pushing data:");
    Serial.println(firebaseData.errorReason());
  }

  delay(5000); // Send every 5 seconds
}
