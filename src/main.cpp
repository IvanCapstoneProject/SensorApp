#define TINY_GSM_MODEM_SIM800

#include <TinyGsmClient.h>
#include <ArduinoHttpClient.h>
#include <HardwareSerial.h>

// Your GPRS credentials (leave empty if not needed)
const char apn[] = "data.lycamobile.mk";
const char user[] = "lmmk";
const char pass[] = "plus";

// SIM800L RX & TX pins
const int RX_PIN = 16; // Connect to SIM800L TX
const int TX_PIN = 17; // Connect to SIM800L RX


// Firebase project URL and Database Secret
const char *firebase_host = "proxyapp-v9lds.ondigitalocean.app";  

TinyGsm modem(Serial2);
TinyGsmClient client(modem); // Use TinyGsmClient for HTTPS
  
HttpClient http(client, firebase_host, 8080); // PROBAJ I DRUGITE PORTI neshto so 4xx beshe 

const int trigPin = 13; // Trigger pin of the ultrasound sensor
const int echoPin = 12; // Echo pin of the ultrasound sensor

void connectToNetwork();

void setup()
{
  Serial.begin(115200);
  Serial2.begin(9600, SERIAL_8N1, RX_PIN, TX_PIN);

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  Serial.println("Connecting...");

  int signalQuality = modem.getSignalQuality();
  Serial.print("Signal Quality: ");
  Serial.println(signalQuality);
  


connectToNetwork();


}

void connectToNetwork()
{
  int attempts = 0;
  const int maxAttempts = 5;
  Serial.println("START");

  while (!modem.gprsConnect(apn, user, pass))
  {
    Serial.println("Failed to connect to cellular network");
    attempts++;
    delay(15000); // Wait for 10 seconds before the next attempt

    if (attempts >= maxAttempts)
    {
      Serial.println("Max connection attempts reached. Exiting...");
      while (1)
      {
        // :(
      }
    }
  }

  Serial.println("Connected to cellular network");

}


void sendToFirebase(const String &path, const String &value) {
  String url = "https://" + String(firebase_host) + path;
  http.connectionKeepAlive();

  // Construct JSON data
  String jsonData = "{\"data\":" + value + "}";

  String contentType = "application/json";

  http.sendHeader("Content-Type", contentType);

  http.post(path, contentType, jsonData);

  int httpResponseCode = http.responseStatusCode();
  String response = http.responseBody();

  Serial.print("url:");
  Serial.println(url);

  Serial.print("jsonData: ");
  Serial.println(jsonData);
  Serial.print("HTTP Response code: ");
  Serial.println(httpResponseCode);
  Serial.println(response);
}


void loop()
{
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // Measure the pulse duration on the echo pin
  long duration = pulseIn(echoPin, HIGH);
  
  // Calculate distance in centimeters
  //float cm = (duration * 0.034) / 2;

  //Serial.print("Distance: ");
  //Serial.print(cm);
  //Serial.println(" cm");
   Serial.println(duration);

  // Convert the distance to a string and send to Firebase
  sendToFirebase("/send-data", String(duration)); // Send data to Firebase


  delay(5000); // Wait for 15 seconds
}