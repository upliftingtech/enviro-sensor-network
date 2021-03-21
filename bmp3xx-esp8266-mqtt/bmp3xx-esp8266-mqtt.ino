
#include <Streaming.h>
#include <Chrono.h>
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include "Adafruit_BMP3XX.h"

// includes for ESP8266 WiFi - https://arduino-esp8266.readthedocs.io/en/latest/esp8266wifi/readme.html#
#include <ESP8266WiFi.h>

#define SEALEVELPRESSURE_HPA (1013.25)

// Constants for WiFi
const char* ssid = "bouncyhouse";
const char* password = "bakabaka";

// Instantiate a Chrono object.
Chrono timeToSample;

// Instantiate WiFi client
WiFiClient wifiClient;

// Instantiate a sensor
Adafruit_BMP3XX bmpSensor;

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
  
  // start wifi
  setup_wifi();
  
  // begin I2C connecton to sensor
  if (!bmpSensor.begin_I2C())
  {
    Serial << "Sensor not found, check wiring." << endl;
    while (1); // loop forever because hardware will need to be fixed and reset
  }
  
  // Adafruit says to do this to set up oversampling and filter initialization
  bmpSensor.setTemperatureOversampling(BMP3_OVERSAMPLING_8X);
  bmpSensor.setPressureOversampling(BMP3_OVERSAMPLING_4X);
  bmpSensor.setIIRFilterCoeff(BMP3_IIR_FILTER_COEFF_3);
  bmpSensor.setOutputDataRate(BMP3_ODR_50_HZ);
}

void loop()
{
  // every 10 seconds take a sample and output to mqtt

  if (timeToSample.hasPassed(10000))
  {
    // reset chrono timer
    timeToSample.restart();
    
    if (! bmpSensor.performReading())
    {
      Serial << "Error reading sensor." << endl;
      return;
    }
    // output pressure and temp to serial
    Serial << "Pressure: " << bmpSensor.pressure / 100 << " hPa" << endl;
    Serial << "Temp: " << bmpSensor.temperature << " C" << endl;
  }
}
