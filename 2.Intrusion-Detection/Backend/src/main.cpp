#include <Arduino.h>
#include <WiFi.h>
#include <FirebaseESP32.h>

// Firebase project credentials
#define FIREBASE_HOST "https://intrusion-detection-c3bd2-default-rtdb.asia-southeast1.firebasedatabase.app"  // Replace with your Firebase project URL
#define FIREBASE_AUTH "Gi1vHfFaXhpJfwRbwebraRNJuPs6mcXKfXk7qCaZ"   // Replace with your Firebase Database Secret

// WiFi credentials
const char* ssid = "HotsSpot";         // Replace with your WiFi SSID
const char* password = "12345678"; // Replace with your WiFi Password

// Define pin numbers
const int trigPin = 23;
const int echoPin = 22;
const int buzzerPin = 19;

// Define sound speed in cm/uS
#define SOUND_SPEED 0.034
#define CM_TO_INCH 0.393701
// Define the distance threshold (in cm)
const int distanceThreshold = 20;

FirebaseData firebaseData;
FirebaseConfig config;
FirebaseAuth auth;

void setup() {
  // Initialize serial monitor
  Serial.begin(9600);
  
  // Configure pin modes
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(buzzerPin, OUTPUT);

  // Connect to WiFi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println(" connected!");

  // Set up Firebase
  config.host = FIREBASE_HOST;
  config.signer.tokens.legacy_token = FIREBASE_AUTH;

  // Initialize Firebase
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
}

void loop() {
  // Clears the trigPin
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  
  // Sets the trigPin on HIGH state for 10 microseconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  // Reads the echoPin, returns the sound wave travel time in microseconds
  long duration = pulseIn(echoPin, HIGH);
  
  // Calculate the distance
  float distance = duration * SOUND_SPEED / 2;
  
  // Print the distance to the serial monitor
  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");
  
  // If the distance is less than the threshold, activate the buzzer
  if (distance < distanceThreshold) {
    digitalWrite(buzzerPin, HIGH);
  } else {
    digitalWrite(buzzerPin, LOW);
  }

  // Send distance data to Firebase
  if (Firebase.setFloat(firebaseData, "/distance", distance)) {
    Serial.println("Distance updated in Firebase");
  } else {
    Serial.print("Failed to update distance, ");
    Serial.println(firebaseData.errorReason());
  }

  // Add a short delay
  delay(500);
}
