#include <Arduino.h>
#include <WiFi.h>
#include <FirebaseESP32.h>  // Firebase library for ESP32

// Wi-Fi credentials
#define WIFI_SSID "HotsSpot"
#define WIFI_PASSWORD "12345678"

// Firebase credentials
#define FIREBASE_HOST "https://lighting-20825-default-rtdb.asia-southeast1.firebasedatabase.app"  // Include 'https://' and trailing '/'
#define FIREBASE_AUTH "o4OjyjHSnbpkSx8hbbdj4ywMLF0CC6D5L9wurQgw"  // Get it from Firebase Console > Database > Secrets

// Pin definitions
#define IR_PIN 34    // Analog pin for IR sensor
#define LED_PIN 5    // GPIO pin for LED
#define IR_THRESHOLD 3000  // Define the threshold for detecting low IR value

FirebaseData firebaseData;
FirebaseConfig firebaseConfig;  // Create FirebaseConfig object
FirebaseAuth firebaseAuth;      // Create FirebaseAuth object

void setup() {
  Serial.begin(115200);
  
  // Set LED pin as output
  pinMode(LED_PIN, OUTPUT);

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
  // Read IR sensor value
  int irValue = analogRead(IR_PIN);

  // Debugging: Print the IR value
  Serial.print("IR Value: ");
  Serial.println(irValue);

  // Send IR value to Firebase
  if (Firebase.setInt(firebaseData, "/lighting_system/ir_value", irValue)) {
    Serial.println("IR Value sent to Firebase");
  } else {
    Serial.println(firebaseData.errorReason());
  }

  // Light up the LED when the IR value is below the threshold (low IR value)
  if (irValue < IR_THRESHOLD) {
    digitalWrite(LED_PIN, HIGH);  // Turn on the LED
    Serial.println("LED ON due to low IR value");
  } else {
    digitalWrite(LED_PIN, LOW);  // Turn off the LED
    Serial.println("LED OFF");
  }

  delay(500);  // Delay for stability
}
