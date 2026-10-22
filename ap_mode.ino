#include <WiFi.h>

const char* ssid = "A0309529W";
const char* password = "YOUR_AP_PASSWORD";

void setup() {
  Serial.begin(115200);
  delay(1000);

  WiFi.mode(WIFI_AP);

  bool success = WiFi.softAP(ssid, password, 1, 0, 2);

  if (success) {
    Serial.println("AP Started Successfully!");
  } else {
    Serial.println("Failed to start AP!");
  }
}

void loop() {
  Serial.print("AP SSID: ");
  Serial.println(ssid);

  Serial.print("AP IP Address: ");
  Serial.println(WiFi.softAPIP());

  Serial.print("AP MAC Address: ");
  Serial.println(WiFi.softAPmacAddress());

  Serial.print("Connected devices: ");
  Serial.println(WiFi.softAPgetStationNum());

  delay(2000);
}
