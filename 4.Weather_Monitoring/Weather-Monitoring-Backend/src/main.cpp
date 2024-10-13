#include <WiFi.h>
#include <FirebaseESP32.h>  // Firebase library for ESP32
#include "DHT.h"

// Wi-Fi credentials
#define WIFI_SSID "HotsSpot"
#define WIFI_PASSWORD "12345678"

// Firebase credentials
#define FIREBASE_HOST "https://weather-monitoring-d6b86-default-rtdb.asia-southeast1.firebasedatabase.app"  // Firebase URL
#define FIREBASE_AUTH "ABOwdVwC6sJfWmp1HpCvmgbtWNAkAarr84fpqeqg"  // Firebase Auth Key

// Pin definitions
#define DHTPIN 15        // GPIO pin for DHT sensor
#define LED_PIN 5        // GPIO pin for LED
#define DHTTYPE DHT11    // DHT11 or DHT22

// Threshold temperature for LED activation
#define TEMP_THRESHOLD 35.0

DHT dht(DHTPIN, DHTTYPE);
FirebaseData firebaseData;
FirebaseConfig firebaseConfig;
FirebaseAuth firebaseAuth;

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);

  // Initialize DHT sensor
  dht.begin();

  // Connect to Wi-Fi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  // Set up Firebase
  firebaseConfig.host = FIREBASE_HOST;
  firebaseConfig.signer.tokens.legacy_token = FIREBASE_AUTH;

  // Initialize Firebase
  Firebase.begin(&firebaseConfig, &firebaseAuth);
  Firebase.reconnectWiFi(true);
}

void loop() {
  // Read temperature and humidity
  float temp = dht.readTemperature();
  float humidity = dht.readHumidity();

  // Check if readings are valid
  if (isnan(temp) || isnan(humidity)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  // Print data to serial monitor
  Serial.print("Temperature: ");
  Serial.print(temp);
  Serial.print(" °C | Humidity: ");
  Serial.print(humidity);
  Serial.println(" %");

  // Send data to Firebase
  if (Firebase.setFloat(firebaseData, "/weather/temperature", temp) &&
      Firebase.setFloat(firebaseData, "/weather/humidity", humidity)) {
    Serial.println("Data sent to Firebase");
  } else {
    Serial.println("Failed to send data to Firebase");
    Serial.println(firebaseData.errorReason());
  }

  // Control LED based on temperature threshold
  if (temp > TEMP_THRESHOLD) {
    digitalWrite(LED_PIN, HIGH);  // Turn on LED
    Serial.println("Temperature exceeds threshold, LED ON");
  } else {
    digitalWrite(LED_PIN, LOW);   // Turn off LED
    Serial.println("Temperature is below threshold, LED OFF");
  }

  // Delay between sensor readings
  delay(2000);
}
