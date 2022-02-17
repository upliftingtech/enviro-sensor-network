
#include <Streaming.h>
#include <Chrono.h>
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include "Adafruit_BMP3XX.h"
#include <PubSubClient.h>       // MQTT

// includes for ESP8266 WiFi - https://arduino-esp8266.readthedocs.io/en/latest/esp8266wifi/readme.html#
#include <ESP8266WiFi.h>

#define SEALEVELPRESSURE_HPA (1013.25)

// Constants for WiFi
const char* ssid = "bouncyhouse";
const char* password = "bakabaka";
const char* mqtt_server = "192.168.2.10";

// global message buffer for mqtt
#define MSG_BUFFER_SIZE	(50)
char msg[MSG_BUFFER_SIZE];

// Instantiate a Chrono object.
Chrono timeToSample;

// Instantiate WiFi client
WiFiClient wifiClient;

// Instantialte an mqtt client
PubSubClient mqttClient(wifiClient);

// Instantiate a sensor
Adafruit_BMP3XX bmpSensor;

// Instantiate an Arduino String class to store our client ID
String clientId = "sensor-";

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
      mqttClient.publish("debug", "hello world");
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
  Serial << endl << "Get data from a BMP3xx sensor!" << endl;
  
  // start wifi
  setup_wifi();
  
// The wifi takes a variable amount of time to connect so using millis
// as a seed here is a good way to initialize the random generator
  randomSeed(millis());

// Create a random client ID
  clientId += String(random(0xffff), HEX);

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
  
  bmpSensor.performReading(); // the first reading seems to be off so do one here to get it out of the way

  // Setup MQTT client
  mqttClient.setServer(mqtt_server, 1883);

}

void loop()
{
  // Connect to MQTT server and reconnect if disconnected
  if (!mqttClient.connected()) reconnect();
  mqttClient.loop(); // non-blocking mqtt background updates
  
  // every 10 seconds take a sample and output to mqtt

  if (timeToSample.hasPassed(30000)) // 30 seconds low res
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
    
    // send temp and pressure to mqtt
    snprintf(msg, MSG_BUFFER_SIZE, "%3.1f", bmpSensor.temperature);
    mqttClient.publish("temp/lowres/outdoor", msg);
    snprintf(msg, MSG_BUFFER_SIZE, "%3.1f", bmpSensor.pressure / 100);
    mqttClient.publish("pressure/002", msg);
  }
}
