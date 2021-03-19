/*
 *  Modified for ESP8266     - Brian Ernst Feb 2021
 *  Added MQTT and WIFI code - Brian Ernst March 2021

    Connections for ESP 8266
    ===========
    Connect SCL to D1
    Connect SDA to D2
    Connect VDD to 3.3V DC
    Connect GROUND to common ground
 */

// INCLUDE

// include
#include <Streaming.h>

// includes for BMP180 temp sensor via I2C
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP085_U.h>

// includes for MQTT via WiFi
#include <ESP8266WiFi.h>        // WiFi
#include <PubSubClient.h>       // MQTT

// include for timer utility Chrono
#include <Chrono.h> 

// Constants for MQTT via WiFi
// Update these with values suitable for your network.

const char* ssid = "bouncyhouse";
const char* password = "bakabaka";
const char* mqtt_server = "192.168.2.10";

// global message buffer for mqtt
#define MSG_BUFFER_SIZE	(50)
char msg[MSG_BUFFER_SIZE];

// Instantiate MQTT and WiFi clients
WiFiClient espClient;
PubSubClient client(espClient);

// Instantiate sensor data object
Adafruit_BMP085_Unified bmp = Adafruit_BMP085_Unified(10085);

// Instantiate a Chrono object.
Chrono timeToSample;

// Instantiate an Arduino String class to store our client ID
String clientId = "sensor-";


//    Displays some basic information on this sensor from the unified
//    sensor API sensor_t type (see Adafruit_Sensor for more information)
//    Only called once from setup()
void displaySensorDetails(void)
{
  sensor_t sensor;
  bmp.getSensor(&sensor);
  Serial << "------------------------------------" << endl;
  Serial << "Sensor:       " << sensor.name << endl;
  Serial << "Driver Ver:   " << sensor.version << endl;
  Serial << "Unique ID:    " << sensor.sensor_id << endl;
  Serial << "Max Value:    " << sensor.max_value << " hPa" << endl;
  Serial << "Min Value:    " << sensor.min_value << " hPa" << endl;
  Serial << "Resolution:   " << sensor.resolution << " hPa" << endl;  
  Serial << "------------------------------------" << endl;
  Serial << endl;
  delay(500);
}

void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial << endl;
  Serial << "Connecting to " << ssid << endl;

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial << ".";
  }

  randomSeed(micros()); 

  Serial << endl;
  Serial << "WiFi connected" << endl;
  Serial << "IP address: " << WiFi.localIP() << endl;
}

// MQTT reconnect function - connects and reconnects to MQTT server
void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial << "Attempting MQTT connection... ";
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial << "connected" << endl;
      // Once connected, publish an announcement...
      client.publish("outTopic", "hello world");
    } 
    else {
      Serial << "failed, rc=" << client.state() << " try again in 5 seconds" << endl;
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup(void) 
{
  Serial.begin(115200);
  Serial << "Pressure Sensor Test" << endl << endl;
  
  /* Initialize the sensor */
  if(!bmp.begin())
  {
    /* There was a problem detecting the BMP085 ... check your connections */
    Serial << "Ooops, no BMP085 detected ... Check your wiring or I2C ADDR!" << endl;
    while(1);
  }
  
  /* Display some basic information on this sensor */
  displaySensorDetails();
  
// Setup the Wifi connection
  setup_wifi();

// The wifi takes a variable amount of time to connect so using millis
// as a seed here is a good way to initialize the random generator
  randomSeed(millis());

// Create a random client ID
  clientId += String(random(0xffff), HEX);

// Setup MQTT client
  client.setServer(mqtt_server, 1883);

}

void loop(void) 
{
  sensors_event_t sensor_event;
 
  // Connect to MQTT server and reconnect if disconnected
  if (!client.connected()) {
    reconnect();
  }
  client.loop(); // non-blocking mqtt background updates

  if (timeToSample.hasPassed(10000)) // returns 1 if 10000 ms (10 seconds) have passed
  {
	  timeToSample.restart();       // restart timer
	  /* Get a new sensor event */ 

	  bmp.getEvent(&sensor_event);
	 
	  /* Display the results (barometric pressure is measure in hPa) */
	  if (sensor_event.pressure)
	  {
	    /* Display atmospheric pressue in hPa */
	    Serial << "Pressure:    " << sensor_event.pressure << endl;
	    
	    /* Calculating altitude with reasonable accuracy requires pressure    *
	     * sea level pressure for your position at the moment the data is     *
	     * converted, as well as the ambient temperature in degress           *
	     * celcius.  If you don't have these values, a 'generic' value of     *
	     * 1013.25 hPa can be used (defined as SENSORS_PRESSURE_SEALEVELHPA   *
	     * in sensors.h), but this isn't ideal and will give variable         *
	     * results from one day to the next.                                  *
	     *                                                                    *
	     * You can usually find the current SLP value by looking at weather   *
	     * websites or from environmental information centers near any major  *
	     * airport.                                                           *
	     *                                                                    *
	     * For example, for Paris, France you can check the current mean      *
	     * pressure and sea level at: http://bit.ly/16Au8ol                   */
	     
	    /* First we get the current temperature from the BMP085 */
	    float temperature;
	    bmp.getTemperature(&temperature);
	    Serial << "Temperature: " << temperature << " C" << endl;
	
	    /* Then convert the atmospheric pressure, and SLP to altitude         */
	    /* Update this next line with the current SLP for better results      */
	    float seaLevelPressure = SENSORS_PRESSURE_SEALEVELHPA;
	    Serial << "Altitude:    "; 
	    Serial << bmp.pressureToAltitude(seaLevelPressure, sensor_event.pressure); 
	    Serial << " m" << endl;
	    
	    // send temp to mqtt
	    snprintf(msg, MSG_BUFFER_SIZE, "%3.1f", temperature);
	    client.publish("sensors/001", msg);

	    // output temp to serial
	    Serial << "Publish message: " << msg << endl;
	  }
	  else
	  {
	    Serial << "Sensor error" << endl;
	  }
	  
  } // endif timeToSample.hasPassed
}
