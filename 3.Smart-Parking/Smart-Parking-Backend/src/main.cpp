#include <Arduino.h>
#include <WiFi.h>
#include <FirebaseESP32.h>  // Firebase library for ESP32

// Wi-Fi credentials
#define WIFI_SSID "HotsSpot"
#define WIFI_PASSWORD "12345678"

// Firebase credentials
#define FIREBASE_HOST "https://smart-lighting-9e99b-default-rtdb.asia-southeast1.firebasedatabase.app/"  // Include 'https://' and trailing '/'
#define FIREBASE_AUTH "ja8bA2rWPM1YieX73eR0FWK90rS6Q3rkQAK97zwZ"  // Get it from Firebase Console > Database > Secrets

// Pin definitions
#define IR_SENSOR_1 34  // Analog pin for IR sensor 1
#define IR_SENSOR_2 35 // Analog pin for IR sensor 2
#define LED_1 2         // GPIO pin for LED 1 (Parking Slot 1)
#define LED_2 5       // GPIO pin for LED 2 (Parking Slot 2)
#define IR_THRESHOLD 3000  // Threshold for detecting a car

FirebaseData firebaseData;
FirebaseConfig firebaseConfig;
FirebaseAuth firebaseAuth;

void setup() {
  Serial.begin(115200);
  
  // Set LED pins as output
  pinMode(LED_1, OUTPUT);
  pinMode(LED_2, OUTPUT);

  // Connect to Wi-Fi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  // Set up Firebase configuration
  firebaseConfig.host = FIREBASE_HOST;
  firebaseConfig.signer.tokens.legacy_token = FIREBASE_AUTH;

  // Initialize Firebase
  Firebase.begin(&firebaseConfig, &firebaseAuth);
  Firebase.reconnectWiFi(true);
}

void updateParkingStatus(const String& slot, int irValue, int ledPin) {
  String path = "/parking_system/" + slot;  // Path in Firebase for this slot
  String status;
  
  if (irValue < IR_THRESHOLD) {
    digitalWrite(ledPin, LOW);  // Car detected (turn off LED)
    status = "Occupied";
    Serial.println(slot + " is Occupied");
  } else {
    digitalWrite(ledPin, HIGH);  // No car detected (turn on LED)
    status = "Available";
    Serial.println(slot + " is Available");
  }

  // Send status to Firebase
  if (Firebase.setString(firebaseData, path, status)) {
    Serial.println("Status sent to Firebase: " + status);
  } else {
    Serial.println(firebaseData.errorReason());
  }
}

void loop() {
  // Read IR sensor values
  int irValue1 = analogRead(IR_SENSOR_1);
  int irValue2 = analogRead(IR_SENSOR_2);

  // Debugging: Print IR sensor values
  Serial.print("IR Sensor 1 Value: ");
  Serial.println(irValue1);
  Serial.print("IR Sensor 2 Value: ");
  Serial.println(irValue2);

  // Update Firebase and control LEDs based on sensor readings
  updateParkingStatus("slot_1", irValue1, LED_1);
  updateParkingStatus("slot_2", irValue2, LED_2);

  delay(1000);  // Delay for stability
}
