#include <WiFi.h>

const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";

void setup() {
  Serial.begin(115200);
  delay(1000);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  Serial.println("Connecting to WiFi...");

  // Stop after 15 seconds.
  int attempts = 0;

  while (WiFi.status() != WL_CONNECTED && attempts < 30) {
    delay(500);
    Serial.print(".");
    attempts++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWiFi Connected!");

    Serial.print("SSID: ");
    Serial.println(WiFi.SSID());

    Serial.print("RSSI: ");
    Serial.print(WiFi.RSSI());
    Serial.println(" dBm");

    Serial.print("STA IP Address: ");
    Serial.println(WiFi.localIP());

    Serial.print("STA MAC Address: ");
    Serial.println(WiFi.macAddress());

  } else {
    Serial.println("\nWiFi connection failed!");
    Serial.println("Check SSID, password and 2.4 GHz WiFi.");
  }
}

void loop() {
}
