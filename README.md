# EE4216 Lab 2

Wi-Fi and HTTP applications on the ESP32-S3.

## Sketches

- `sta_connection`: connects to a 2.4 GHz network with a 15-second timeout and prints the SSID, RSSI, IP and MAC address.
- `ap_mode`: starts the AP with a maximum of two clients and prints the AP information.
- `http_dashboard`: serves the supplied dashboard, measures distance on request and controls eight LEDs independently.

Open each `.ino` from its own folder. Set the SSID and password before uploading. The passwords in this repository are placeholders. Use an AP password of at least eight characters.

## Setup

Board: ESP32S3 Dev Module. Serial Monitor: 115200 baud.

Install Arduino-ESP32, NewPing, Adafruit NeoPixel and ArduinoJson 7. WiFi and WebServer are included with Arduino-ESP32.

| Connection | Pin |
| --- | --- |
| HC-SR04 TRIG | GPIO4 |
| HC-SR04 ECHO | GPIO5 through the Lab 1 voltage divider |
| WS2812 data input | GPIO6 |

Use the Lab 1 circuit with a shared ground. Keep `lab_page.h` beside `http_dashboard.ino`; it is the supplied Lab 2 dashboard.

## HTTP interface

Connect to the AP and open `http://192.168.4.1/` (or use the AP IP printed in Serial Monitor).

| Method | Path | Purpose |
| --- | --- | --- |
| GET | `/` | Dashboard |
| GET | `/api/distance` | One new ultrasonic reading |
| GET | `/api/leds` | Read eight saved LED states |
| PUT | `/api/leds/{id}` | Update one LED, ID 0 to 7 |

PUT body: `{"r":255,"g":0,"b":0,"brightness":128}`.

Successful API requests return HTTP 200 and JSON. Invalid fields return 400; unknown LED IDs or routes return 404. Errors leave the LED states unchanged. RGB and brightness must be integers from 0 to 255. Per-pixel brightness scales each channel by `brightness / 255`.

A distance request uses NewPing with a 200 cm limit and a finite echo timeout. No valid echo returns `{"distance_cm":null,"valid":false}`. JSON responses use `Cache-Control: no-store`.

## Checks

After flashing, request distance, load the LED settings, and apply different colours and brightness values to several LEDs. Change one LED and check that the other seven remain unchanged. Use an HTTP client to check invalid JSON, out-of-range fields and an unknown ID. For an invalid sensor reading, remove the target or place it outside the sensor range.

The HTTP handlers run synchronously inside `server.handleClient()`. The sensor wait finishes or times out before another handler runs.
