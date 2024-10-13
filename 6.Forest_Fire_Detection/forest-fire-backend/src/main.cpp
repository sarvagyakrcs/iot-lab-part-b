#include <WiFi.h>          // For connecting to WiFi
#include <FirebaseESP32.h> // Firebase library for ESP32
#include <ESP32Servo.h>    // ESP32 compatible Servo library

// Wi-Fi credentials
#define WIFI_SSID "HotsSpot"
#define WIFI_PASSWORD "12345678"

// Firebase credentials
#define FIREBASE_HOST "https://smart-irrigation-f734e-default-rtdb.asia-southeast1.firebasedatabase.app"
#define FIREBASE_AUTH "ksJe7PNHxtfDBXo1aWDaGV2EhhoLWpfNxhW9v1YW"

// Pin Definitions
#define MQ2PIN 34          // GPIO pin for MQ2 sensor (smoke sensor)
#define LED_PIN 5          // GPIO pin for LED
#define SMOKE_THRESHOLD 800  // Smoke level threshold for detection

FirebaseData firebaseData;    // Firebase object
FirebaseConfig firebaseConfig; 
FirebaseAuth firebaseAuth;

void connectToWiFi() {
  // Start connecting to WiFi
  Serial.print("Connecting to WiFi");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  // Wait until connected
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }

  // Print IP address after connection
  Serial.println();
  Serial.print("Connected to WiFi. IP Address: ");
  Serial.println(WiFi.localIP());
}

void setup() {
  Serial.begin(115200);

  // Initialize LED pin
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);  // Initially turn off LED

  // Connect to WiFi
  connectToWiFi();

  // Firebase configuration setup
  firebaseConfig.host = FIREBASE_HOST;
  firebaseConfig.signer.tokens.legacy_token = FIREBASE_AUTH;

  // Initialize Firebase
  Firebase.begin(&firebaseConfig, &firebaseAuth);
  Firebase.reconnectWiFi(true);
}

void loop() {
  // Read smoke level from MQ2 sensor
  int smokeLevel = analogRead(MQ2PIN);

  // Print the sensor data to the serial monitor
  Serial.print("Smoke Level: ");
  Serial.println(smokeLevel);

  // Upload smoke level to Firebase
  if (Firebase.setInt(firebaseData, "/sensorData/smokeLevel", smokeLevel)) {
    Serial.println("Smoke data sent to Firebase");
  } else {
    Serial.println("Failed to send smoke data to Firebase");
    Serial.println(firebaseData.errorReason());
  }

  // Check if smoke level exceeds threshold (fire detected)
  if (smokeLevel > SMOKE_THRESHOLD) {
    Firebase.setString(firebaseData, "/status", "FIRE DETECTED");
    Serial.println("ALERT: FIRE DETECTED!");
    
    // Turn on LED when smoke is detected
    digitalWrite(LED_PIN, HIGH); // LED ON
  } else {
    Firebase.setString(firebaseData, "/status", "Normal");
    Serial.println("Status: Normal");
    
    // Turn off LED when no smoke is detected
    digitalWrite(LED_PIN, LOW); // LED OFF
  }

  // Delay before next reading
  delay(2000); // Wait for 2 seconds before the next reading
}
