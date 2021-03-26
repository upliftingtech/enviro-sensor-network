#include <Streaming.h>
#include <Chrono.h>
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
Adafruit_TSL2561_Unified tsl = Adafruit_TSL2561_Unified(TSL2561_ADDR_FLOAT, 12345);

// Instantiate a Chrono object.
Chrono timeToSample;


/**************************************************************************/
/*
    Configures the gain and integration time for the TSL2561
    From Adafruit example code
*/
/**************************************************************************/
void configureSensor(void)
{
  /* You can also manually set the gain or enable auto-gain support */
  // tsl.setGain(TSL2561_GAIN_1X);      /* No gain ... use in bright light to avoid sensor saturation */
  // tsl.setGain(TSL2561_GAIN_16X);     /* 16x gain ... use in low light to boost sensitivity */
  tsl.enableAutoRange(true);            /* Auto-gain ... switches automatically between 1x and 16x */
  
  /* Changing the integration time gives you better sensor resolution (402ms = 16-bit data) */
  // tsl.setIntegrationTime(TSL2561_INTEGRATIONTIME_13MS);      /* fast but low resolution */
  // tsl.setIntegrationTime(TSL2561_INTEGRATIONTIME_101MS);  /* medium resolution and speed   */
  tsl.setIntegrationTime(TSL2561_INTEGRATIONTIME_402MS);  /* 16-bit data but slowest conversions */

  /* Update these values depending on what you've set above! */  
  Serial.println("------------------------------------");
  Serial.print  ("Gain:         "); Serial.println("Auto");
  Serial.print  ("Timing:       "); Serial.println("402 ms. Slow 16-bit data.");
  Serial.println("------------------------------------");
}


void setup()
{
    Serial.begin(115200);
    
    Serial << endl << "Sensor test." << endl;
    
    if(!tsl.begin())
    {
      /* There was a problem detecting the TSL2561 ... check your connections */
      Serial << "Ooops, no TSL2561 detected ... Check your wiring or I2C ADDR!" << endl;
      while(1);
    }

    configureSensor();

}

void loop()
{
    sensors_event_t event;

    if (timeToSample.hasPassed(10000))
  {
    // reset chrono timer
    timeToSample.restart();
    
    // Get a new sensor event
    tsl.getEvent(&event);
    
    /* Display the results (light is measured in lux) */
    if (event.light)
    {
      Serial << event.light << " lux" << endl;
    }
    else
    {
      /* If event.light = 0 lux the sensor is probably saturated
         and no reliable data could be generated! */
      Serial << "Sensor overload" << endl;
    }
  }
}
