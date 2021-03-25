#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_TSL2561_U.h>

/* From Adafruit example code:
  You should also assign a unique ID to this sensor for use with
  the Adafruit Sensor API so that you can identify this particular
  sensor in any data logs, etc.  To assign a unique ID, simply
  provide an appropriate value in the constructor below (12345
  is used by default in this example).
  
  The address will be different depending on whether you leave
  the ADDR pin float (addr 0x39), or tie it to ground or vcc. In those cases
  use TSL2561_ADDR_LOW (0x29) or TSL2561_ADDR_HIGH (0x49) respectively
*/
Adafruit_TSL2561 lightSensor = Adafruit_TSL2561(TSL2561_ADDR_FLOAT, 12345);

void setup();
{
}

void loop();
{
}
