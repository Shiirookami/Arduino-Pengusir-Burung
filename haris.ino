#include <ESP8266WiFi.h>
#include <FirebaseESP8266.h>
#include <Servo.h>

// Your WiFi credentials
const char* ssid = "Redmi";
const char* password = "12345678";

// Firebase database URL and API key
#define FIREBASE_HOST "https://harisemhas-default-rtdb.firebaseio.com"
#define FIREBASE_AUTH "NlbQvuC5Ews8uHSAZK5XW1iovKfLFInPqSgZW7iI"

FirebaseData firebaseData;
FirebaseAuth auth;
FirebaseConfig config;

const int TRIG_PIN = D5;
const int ECHO_PIN = D6;
const int SERVO_PIN = D4;

Servo myservo;

void setup() {
  Serial.begin(115200);
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  myservo.attach(SERVO_PIN);
  myservo.write(0);

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  // Configure Firebase
  config.host = FIREBASE_HOST;
  config.signer.tokens.legacy_token = FIREBASE_AUTH;

  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
}

void loop() {
  long duration, distance;

  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  duration = pulseIn(ECHO_PIN, HIGH);
  distance = (duration / 2) / 29.1;

  if (distance < 20) {
    myservo.write(180);
  } else {
    myservo.write(0);
  }

  // Send data to Firebase
  if (Firebase.setInt(firebaseData, "/sensor/distance", distance)) {
    Serial.println("Data sent to Firebase");
  } else {
    Serial.print("Error sending data: ");
    Serial.println(firebaseData.errorReason());
  }

  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");

  delay(1000);
}
