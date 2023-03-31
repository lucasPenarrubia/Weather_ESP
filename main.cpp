//
// A simple server implementation showing how to:
//  * serve static messages
//  * read GET and POST parameters
//  * handle missing pages / 404s
//

#include <Arduino.h>
#ifdef ESP32
#include <WiFi.h>
#include <AsyncTCP.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#endif
#include <ESPAsyncWebServer.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <Wire.h>
#include <Adafruit_BMP280.h>

AsyncWebServer server(80);

Adafruit_BMP280 bmp;

const char* ssid = "Bbox-B0181C91";
const char* password = "V3jDS2KQY6i5i2412v";

const char* PARAM_MESSAGE = "message";

void notFound(AsyncWebServerRequest *request) {
    request->send(404, "text/plain", "Not found");
}

const int PHOTODETECTOR_PIN = 34;
const int DHT_PIN = 26;
DHT dht(DHT_PIN, DHT22);
float temp;
float temp2;
float hum;
float altitude;
float pressure;
unsigned long previousMillis = 0;
const long interval = 5000;
int lum;

void setup() {
Wire.begin();
Serial.begin(115200);
if (!bmp.begin(0x76)) {
  Serial.println("Could not find a valid BMP280 sensor, check wiring!");
  while (1);
}

pinMode(PHOTODETECTOR_PIN, INPUT);
dht.begin();

Serial.begin(115200);
WiFi.mode(WIFI_STA);
WiFi.begin(ssid, password);
if (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.printf("WiFi Failed!\n");
    return;
}

Serial.print("IP Address: ");
Serial.println(WiFi.localIP());

server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "text/plain", "Server OK");
});

// Send a GET request to <IP>/get?message=<message>
server.on("/temperature", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(200, "text/plain", String(temp, 1));
});

server.on("/humidite", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(200, "text/plain", String(hum, 1));
});

server.on("/lum", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(200, "text/plain", String(lum, 2));
});

server.on("/temperature2", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(200, "text/plain", String(temp2, 2));
});

server.on("/pressure", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(200, "text/plain", String(pressure, 2));
});

server.on("/alt", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(200, "text/plain", String(altitude, 2));
});

// Send a POST request to <IP>/post with a form field message set to <message>
server.on("/post", HTTP_POST, [](AsyncWebServerRequest *request){
    String message;
    if (request->hasParam(PARAM_MESSAGE, true)) {
        message = request->getParam(PARAM_MESSAGE, true)->value();
    } else {
        message = "No message sent";
    }
    request->send(200, "text/plain", "Hello, POST: " + message);
});

server.onNotFound(notFound);

server.begin();
}

void loop() {
  if (millis() - previousMillis >= interval) {
    previousMillis = millis();
    temp = dht.readTemperature();
    hum = dht.readHumidity();
    lum = digitalRead(PHOTODETECTOR_PIN);
    temp2 = bmp.readTemperature();
    pressure = bmp.readPressure()/100.0F;
    altitude = bmp.readAltitude(1013.25);
  }
}

