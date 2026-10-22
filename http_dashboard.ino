#include <WiFi.h>
#include <WebServer.h>
#include <Adafruit_NeoPixel.h>
#include <NewPing.h>
#include <ArduinoJson.h>
#include "lab_page.h"

const char* ssid = "A0309529W";
const char* password = "YOUR_AP_PASSWORD";

#define TRIG_PIN 4
#define ECHO_PIN 5
#define LED_PIN 6
#define NUM_LEDS 8
#define MAX_DISTANCE 200

WebServer server(80);
NewPing sonar(TRIG_PIN, ECHO_PIN, MAX_DISTANCE);
Adafruit_NeoPixel strip(NUM_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);

struct LEDState {
  uint8_t r;
  uint8_t g;
  uint8_t b;
  uint8_t brightness;
};

LEDState leds[NUM_LEDS] = {};

void sendJson(int status, JsonDocument& doc) {
  String output;
  serializeJson(doc, output);
  server.sendHeader("Cache-Control", "no-store");
  server.send(status, "application/json", output);
}

void sendError(int status, const char* message) {
  JsonDocument doc;
  doc["error"] = message;
  sendJson(status, doc);
}

// Apply per-LED brightness.
void updateStrip() {
  for (int i = 0; i < NUM_LEDS; i++) {
    LEDState s = leds[i];

    uint8_t r = (uint16_t)s.r * s.brightness / 255;
    uint8_t g = (uint16_t)s.g * s.brightness / 255;
    uint8_t b = (uint16_t)s.b * s.brightness / 255;

    strip.setPixelColor(i, strip.Color(r, g, b));
  }

  strip.show();
}

void addLedJson(JsonObject obj, int id) {
  obj["id"] = id;
  obj["r"] = leds[id].r;
  obj["g"] = leds[id].g;
  obj["b"] = leds[id].b;
  obj["brightness"] = leds[id].brightness;
}

// Dashboard
void handleRoot() {
  server.send_P(200, "text/html", LAB_PAGE);
}

// One reading per request.
void handleDistance() {
  unsigned int distance = sonar.ping_cm();

  JsonDocument doc;

  if (distance == 0) {
    doc["distance_cm"] = nullptr;
    doc["valid"] = false;
  } else {
    doc["distance_cm"] = distance;
    doc["valid"] = true;
  }

  sendJson(200, doc);
}

// Read all LED states.
void handleGetLeds() {
  JsonDocument doc;
  JsonArray array = doc["leds"].to<JsonArray>();

  for (int i = 0; i < NUM_LEDS; i++) {
    JsonObject obj = array.add<JsonObject>();
    addLedJson(obj, i);
  }

  sendJson(200, doc);
}

bool readField(JsonDocument& doc, const char* key, int& value) {
  if (!doc[key].is<int>()) {
    return false;
  }

  value = doc[key].as<int>();
  return value >= 0 && value <= 255;
}

// Update one LED.
void handlePutLed(int id) {
  JsonDocument request;

  DeserializationError error =
      deserializeJson(request, server.arg("plain"));

  if (error || !request.is<JsonObject>() ||
      request.size() != 4) {
    sendError(400, "Invalid JSON body");
    return;
  }

  int r, g, b, brightness;

  if (!readField(request, "r", r) ||
      !readField(request, "g", g) ||
      !readField(request, "b", b) ||
      !readField(request, "brightness", brightness)) {
    sendError(400, "RGB and brightness must be integers from 0 to 255");
    return;
  }

  leds[id] = {
    (uint8_t)r,
    (uint8_t)g,
    (uint8_t)b,
    (uint8_t)brightness
  };

  updateStrip();

  JsonDocument response;
  JsonObject obj = response.to<JsonObject>();
  addLedJson(obj, id);

  sendJson(200, response);
}

void handleOtherRequests() {
  String path = server.uri();
  String prefix = "/api/leds/";

  if (server.method() == HTTP_PUT &&
      path.startsWith(prefix)) {

    String idText = path.substring(prefix.length());

    if (idText.length() != 1 ||
        idText[0] < '0' || idText[0] > '7') {
      sendError(404, "Unknown LED ID");
      return;
    }

    int id = idText[0] - '0';
    handlePutLed(id);
    return;
  }

  sendError(404, "Unknown endpoint");
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  strip.begin();
  strip.clear();
  strip.show();
  WiFi.mode(WIFI_AP);

  if (!WiFi.softAP(ssid, password, 1, 0, 2)) {
    Serial.println("Failed to start WiFi AP");
    return;
  }

  Serial.println("WiFi AP started!");
  Serial.print("AP IP Address: ");
  Serial.println(WiFi.softAPIP());
  Serial.print("AP MAC Address: ");
  Serial.println(WiFi.softAPmacAddress());

  server.on("/", HTTP_GET, handleRoot);
  server.on("/api/distance", HTTP_GET, handleDistance);
  server.on("/api/leds", HTTP_GET, handleGetLeds);

  server.onNotFound(handleOtherRequests);

  server.begin();

  Serial.println("HTTP Server started!");
}

void loop() {
  server.handleClient();
  delay(2);
}
