
#include <Streaming.h>
#include <Chrono.h>
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include "Adafruit_BMP3XX.h"

// Instantiate a Chrono object.
Chrono timeToSample;

void setup()
{
	Serial.begin(115200);
	Serial << endl << "Get data from a BMP3xx sensor!" << endl;
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
