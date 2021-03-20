
#include <Streaming.h>
#include <Chrono.h>
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include "Adafruit_BMP3XX.h"

// Constants for WiFi
const char* ssid = "bouncyhouse";
const char* password = "bakabaka";

// includes for ESP8266 WiFi - https://arduino-esp8266.readthedocs.io/en/latest/esp8266wifi/readme.html#
#include <ESP8266WiFi.h>

// Instantiate a Chrono object.
Chrono timeToSample;

// Instantiate WiFi client
WiFiClient wifiClient;

void setup_wifi() 
{
  // Connecting to a WiFi network
  Serial << endl;
  Serial << "Connecting to " << ssid << endl;

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial << ".";
  }

  Serial << endl;
  Serial << "WiFi connected" << endl;
  Serial << "IP address: " << WiFi.localIP() << endl;
}

void setup()
{
  Serial.begin(115200);
  Serial << endl << "Get data from a BMP3xx sensor!" << endl;
  
  setup_wifi();
}

void loop()
{
  // every 10 seconds take a sample and output to mqtt

  if (timeToSample.hasPassed(10000))
  {
    // reset chrono timer
    timeToSample.restart();
    Serial << "this would be a sample..." << endl;
  }
}
