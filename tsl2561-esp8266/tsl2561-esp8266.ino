#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_TSL2561_U.h>

// From Adafruit example code:
// The address will be different depending on whether you leave
// the ADDR pin float (addr 0x39), or tie it to ground or vcc. In those cases
// use TSL2561_ADDR_LOW (0x29) or TSL2561_ADDR_HIGH (0x49) respectively
Adafruit_TSL2561 lightSensor = Adafruit_TSL2561(TSL2561_ADDR_FLOAT, 12345);

void setup();
{
}

void loop();
{
}
