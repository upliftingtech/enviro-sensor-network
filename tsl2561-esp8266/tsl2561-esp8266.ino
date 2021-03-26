#include <Streaming.h>
#include <Chrono.h>
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_TSL2561_U.h>
#include <PubSubClient.h>       // MQTT

// includes for ESP8266 WiFi - https://arduino-esp8266.readthedocs.io/en/latest/esp8266wifi/readme.html#
#include <ESP8266WiFi.h>

// Constants for WiFi
const char* ssid = "bouncyhouse";
const char* password = "bakabaka";
const char* mqtt_server = "192.168.2.10";

// global message buffer for mqtt
#define MSG_BUFFER_SIZE	(50)
char msg[MSG_BUFFER_SIZE];

// Instantiate WiFi client
WiFiClient wifiClient;

// Instantialte an mqtt client
PubSubClient mqttClient(wifiClient);

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

// Instantiate an Arduino String class to store our client ID
String clientId = "sensor-";


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

// MQTT reconnect function - connects and reconnects to MQTT server
void reconnect() {
  // Loop until we're reconnected
  while (!mqttClient.connected()) {
    Serial << "Attempting MQTT connection... ";
    // Attempt to connect
    if (mqttClient.connect(clientId.c_str())) {
      Serial << "connected" << endl;
      // Once connected, publish an announcement...
      mqttClient.publish("outTopic", "hello world");
    } 
    else {
      Serial << "failed, rc=" << mqttClient.state() << " try again in 5 seconds" << endl;
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}


void setup()
{
    Serial.begin(115200);
    
    Serial << endl << "Sensor test." << endl;
    
  // start wifi
  setup_wifi();
  
// The wifi takes a variable amount of time to connect so using millis
// as a seed here is a good way to initialize the random generator
  randomSeed(millis());

// Create a random client ID
  clientId += String(random(0xffff), HEX);

    if(!tsl.begin())
    {
      /* There was a problem detecting the TSL2561 ... check your connections */
      Serial << "Ooops, no TSL2561 detected ... Check your wiring or I2C ADDR!" << endl;
      while(1);
    }

    configureSensor();

  // Setup MQTT client
  mqttClient.setServer(mqtt_server, 1883);

}

void loop()
{
    sensors_event_t event;
    
      // Connect to MQTT server and reconnect if disconnected
  if (!mqttClient.connected()) reconnect();
  mqttClient.loop(); // non-blocking mqtt background updates


  if (timeToSample.hasPassed(10000))
  {
    // reset chrono timer
    timeToSample.restart();
    
    // Get a new sensor event
    tsl.getEvent(&event);
    
    /* Display the results (light is measured in lux) */
    
    Serial << event.light << " lux" << endl;
    
    // send temp and pressure to mqtt
    snprintf(msg, MSG_BUFFER_SIZE, "%3.1f", event.light);
    mqttClient.publish("light/001", msg);
  }
}
