#include <ESP8266WiFi.h>
#include <WiFiClient.h>

/* ------------ WiFi ------------ */
const char* ssid = "YOUR_WIFI_NAME";
const char* password = "YOUR_WIFI_PASSWORD";

/* ------------ ThingSpeak ------------ */
const char* server = "api.thingspeak.com";
String apiKey = "YOUR_WRITE_API_KEY";

/* ------------ Pins ------------ */
#define TRIG_PIN D1
#define ECHO_PIN D2
#define SOUND_PIN A0
#define LED_PIN D0

/* ------------ Thresholds ------------ */
#define DIST_LIMIT 20
#define SOUND_LIMIT 600

WiFiClient client;

/* ------------ Functions ------------ */
float readDistance() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH, 25000);
  if (duration == 0) return 999;
  return duration * 0.034 / 2;
}

/* ------------ Setup ------------ */
void setup() {
  Serial.begin(9600);

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(LED_PIN, OUTPUT);

  WiFi.begin(ssid, password);
  Serial.print("Connecting");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi Connected");
}

/* ------------ Loop ------------ */
void loop() {

  float distance = readDistance();
  int sound = analogRead(SOUND_PIN);

  int intrusion = (distance < DIST_LIMIT && sound > SOUND_LIMIT) ? 1 : 0;
  digitalWrite(LED_PIN, intrusion ? HIGH : LOW);

  Serial.println("Distance: " + String(distance));
  Serial.println("Sound: " + String(sound));
  Serial.println("Intrusion: " + String(intrusion));

  if (client.connect(server, 80)) {

    String url = "/update?api_key=" + apiKey;
    url += "&field1=" + String(distance);
    url += "&field2=" + String(sound);
    url += "&field3=" + String(intrusion);

    client.print(String("GET ") + url + " HTTP/1.1\r\n" +
                 "Host: " + server + "\r\n" +
                 "Connection: close\r\n\r\n");
  }

  client.stop();

  delay(16000);   // ThingSpeak minimum delay
}
