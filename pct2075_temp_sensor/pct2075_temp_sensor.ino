#include <Streaming.h>
#include <Chrono.h>
#include <Adafruit_PCT2075.h>
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


Adafruit_PCT2075 PCT2075;

// Instantiate Chrono objects.
Chrono timeToHighresSample;
Chrono timeToLowresSample;

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
      mqttClient.publish("outTopic", "hello world");
    } 
    else {
      Serial << "failed, rc=" << mqttClient.state() << " try again in 5 seconds" << endl;
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}


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

  // start wifi
  setup_wifi();
  
// The wifi takes a variable amount of time to connect so using millis
// as a seed here is a good way to initialize the random generator
  randomSeed(millis());

// Create a random client ID
  clientId += String(random(0xffff), HEX);
  
  // Setup MQTT client
  mqttClient.setServer(mqtt_server, 1883);


}

void loop() {
  // Connect to MQTT server and reconnect if disconnected
  if (!mqttClient.connected()) reconnect();
  mqttClient.loop(); // non-blocking mqtt background updates

if (timeToHighresSample.hasPassed(1000)) // one second sample for a higer res less amount of time display
  {
  // reset chrono timer
  timeToHighresSample.restart();
  
  Serial << "Temperature: " << PCT2075.getTemperature() << " C" << endl;
  
    // send temp to mqtt
    snprintf(msg, MSG_BUFFER_SIZE, "%3.2f", PCT2075.getTemperature());
    mqttClient.publish("indoortemp/highres/002", msg);

  }
  
  if (timeToLowresSample.hasPassed(30000)) // 30 seconds sample for a lower res more time display
  {
    // reset chrono timer
    timeToLowresSample.restart();
  
    // send temp to mqtt
    snprintf(msg, MSG_BUFFER_SIZE, "%3.2f", PCT2075.getTemperature());
    mqttClient.publish("indoortemp/lowres/002", msg);
  }
} // end loop()
