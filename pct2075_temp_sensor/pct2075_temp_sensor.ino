#include <Streaming.h>
#include <Chrono.h>
    #include <Adafruit_PCT2075.h>
     
     
    Adafruit_PCT2075 PCT2075;
    
// Instantiate a Chrono object.
Chrono timeToSample;

     
    void setup() {
      PCT2075 = Adafruit_PCT2075();
     
      Serial.begin(115200);
      // Wait until serial port is opened
      while (!Serial) { delay(1); }
      Serial.println("Adafruit PCT2075 Test");
     
      if (!PCT2075.begin()) {
        Serial.println("Couldn't find PCT2075 chip");
        while (1);
      }
      
      Serial.println("Found PCT2075 chip");
     
    }
     
    void loop() {
        if (timeToSample.hasPassed(10000))
  {
    // reset chrono timer
    timeToSample.restart();

      Serial.print("Temperature: "); Serial.print(PCT2075.getTemperature());Serial.println(" C");
      
    }
    } // end loop()
