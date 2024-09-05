#include <WiFi.h>
#include <Firebase_ESP_Client.h>
#include "MHZ19.h"

// Firebase project credentials
#define API_KEY "AIzaSyAS_rVBCn2vIKNH7Vz4vriES78rrySmFh8"
#define DATABASE_URL "https://ccus-detection-default-rtdb.firebaseio.com/" 

FirebaseData firebaseData;
FirebaseConfig firebaseConfig;
FirebaseAuth firebaseAuth;

// Replace with your network credentials
const char* ssid = "MTN_4G_05060B";
const char* password = "maryjane";

// MHZ19B object
MHZ19 myMHZ19;
HardwareSerial mySerial(2);  // Use UART2 on ESP32

// Define LED pins
const int redLedPin = 18;
const int greenLedPin = 19;

// Define constants
#define BAUDRATE 9600
#define RX_PIN 16
#define TX_PIN 17

// Variables to store sensor data
int co2 = 0;
int8_t temp = 0;

// Timer for data retrieval
unsigned long getDataTimer = 0;

void setup() {
  // Start serial communication
  Serial.begin(9600);

  // Initialize Firebase configuration
  firebaseConfig.api_key = API_KEY;
  firebaseConfig.database_url = DATABASE_URL;
  
  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  // Initialize Firebase
  Firebase.begin(&firebaseConfig, &firebaseAuth);
  Firebase.reconnectWiFi(true);

  // Authenticate anonymously
  if (Firebase.signUp(&firebaseConfig, &firebaseAuth, "", "")) {
    Serial.println("Anonymous authentication successful.");
  } else {
    Serial.printf("Anonymous authentication failed: %s\n", firebaseData.errorReason().c_str());
  }

  // Initialize LED pins
  pinMode(redLedPin, OUTPUT);
  pinMode(greenLedPin, OUTPUT);

  // Initialize the MHZ19B sensor
  mySerial.begin(BAUDRATE, SERIAL_8N1, RX_PIN, TX_PIN);  // Initialize UART2 with RX and TX pins
  myMHZ19.begin(mySerial);  // Initialize the MHZ19 sensor
  myMHZ19.autoCalibration(false);  // Disable auto-calibration
}

void loop() {
  // Check if enough time has passed to take a new reading
  if (millis() - getDataTimer >= 2000) {
    // Read sensor data
    co2 = myMHZ19.getCO2();
    temp = myMHZ19.getTemperature();

    Serial.printf("CO2: %d ppm, Temp: %d C\n", co2, temp);

    // Send data to Firebase
    if (Firebase.RTDB.setInt(&firebaseData, "/co2", co2)) {
      Serial.println("CO2 data sent to Firebase.");
    } else {
      Serial.println("Failed to send CO2 data: " + firebaseData.errorReason());
    }

    if (Firebase.RTDB.setInt(&firebaseData, "/temperature", temp)) {
      Serial.println("Temperature data sent to Firebase.");
    } else {
      Serial.println("Failed to send temperature data: " + firebaseData.errorReason());
    }

    // Control LEDs based on CO2 levels
    if (co2 > 1300) {
      digitalWrite(redLedPin, HIGH);
      digitalWrite(greenLedPin, LOW);
    } else {
      digitalWrite(redLedPin, LOW);
      digitalWrite(greenLedPin, HIGH);
    }

    // Update the timer
    getDataTimer = millis();
  }
}
