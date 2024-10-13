#include <WiFi.h>           // For connecting to WiFi
#include <DHT.h>            // For DHT11 sensor
#include <FirebaseESP32.h>  // Firebase library for ESP32
#include <ESP32Servo.h>     // ESP32 compatible Servo library

// Wi-Fi credentials
#define WIFI_SSID "HotsSpot"
#define WIFI_PASSWORD "12345678"

// Firebase credentials
#define FIREBASE_HOST "https://smart-irrigation-f734e-default-rtdb.asia-southeast1.firebasedatabase.app"
#define FIREBASE_AUTH "ksJe7PNHxtfDBXo1aWDaGV2EhhoLWpfNxhW9v1YW"

// Pin Definitions
#define DHTPIN 14            // GPIO pin for DHT sensor
#define SERVO_PIN 15         // GPIO pin for Servo motor
#define DHTTYPE DHT11        // DHT11 or DHT22

DHT dht(DHTPIN, DHTTYPE);    // Initialize DHT sensor
Servo irrigationServo;       // Initialize Servo motor

FirebaseData firebaseData;   // Firebase object
FirebaseConfig firebaseConfig; 
FirebaseAuth firebaseAuth;

// Thresholds
const float TEMP_THRESHOLD = 35.0;    // Temperature threshold in °C
const float HUMIDITY_THRESHOLD = 50.0;  // Humidity threshold in %

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

  // Initialize DHT sensor
  dht.begin();

  // Initialize Servo motor
  irrigationServo.attach(SERVO_PIN);
  irrigationServo.write(0);  // Initially closed (0 degrees)

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
  // Read temperature and humidity from DHT sensor
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();

  // Check if sensor readings are valid
  if (isnan(temperature) || isnan(humidity)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  // Display data
  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.print(" °C, Humidity: ");
  Serial.print(humidity);
  Serial.println(" %");

  // Send data to Firebase
  if (Firebase.setFloat(firebaseData, "/sensorData/temperature", temperature) &&
      Firebase.setFloat(firebaseData, "/sensorData/humidity", humidity)) {
    Serial.println("Data sent to Firebase");
  } else {
    Serial.println("Failed to send data to Firebase");
    Serial.println(firebaseData.errorReason());
  }

  // Control the servo motor based on temperature and humidity thresholds
  if (temperature > TEMP_THRESHOLD || humidity < HUMIDITY_THRESHOLD) {
    irrigationServo.write(90);  // Open irrigation system (90 degrees)
    if (Firebase.setString(firebaseData, "/irrigationStatus", "Irrigation ON")) {
      Serial.println("Irrigation ON");
    }
  } else {
    irrigationServo.write(0);  // Close irrigation system (0 degrees)
    if (Firebase.setString(firebaseData, "/irrigationStatus", "Irrigation OFF")) {
      Serial.println("Irrigation OFF");
    }
  }

  // Delay before next sensor reading
  delay(2000);
}
