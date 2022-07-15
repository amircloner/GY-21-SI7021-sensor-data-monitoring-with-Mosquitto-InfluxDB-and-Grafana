#define XSTR(x) #x
#define STR(x) XSTR(x)

#include <Arduino.h>
#include <GY21.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#define MQTT_TOPIC_HUMIDITY "home/GY-21-SI7021/humidity"
#define MQTT_TOPIC_TEMPERATURE "home/GY-21-SI7021/temperature"
#define MQTT_PUBLISH_DELAY 5000
const char *MQTT_TOPIC_STATE = "home/GY-21-SI7021/status";
const char *MQTT_WILL_MESSAGE = "disconnect";

// Set WiFi credentials
const char *WIFI_SSID = "AndroidAP";     // Enter your WiFi name
const char *WIFI_PASSWORD = "2929116mm"; // Enter WiFi password
// MQTT Broker
const char *MQTT_SERVER = STR(MQTT_BROKER); // Enter MQTT broker address
const char *MQTT_USER = STR(MQTT_USERNAME);
const char *MQTT_PASS = STR(MQTT_PASSWORD);
const int MQTT_PORT = 1883;
String MQTT_CLIENT_ID = "esp8266-client-" + String(WiFi.macAddress());

#ifdef ESP8266
#define SCL 14 // D5 ON NODEMCU
#define SDA 12 // D6 ON NODEMCU
#endif

// Connect VIN to 3-5VDC
// Connect GND to ground
// Connect SCL to I2C clock pin
// Connect SDA to I2C data pin
GY21 sensor;

float humidity;
float temperature;
long lastMsgTime = 0;

WiFiClient espClient;
PubSubClient mqttClient(espClient);

void setupWifi()
{
  Serial.print("Connecting to ");
  Serial.println(WIFI_SSID);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void mqttReconnect()
{
  while (!mqttClient.connected())
  {
    Serial.print("Attempting MQTT connection...");

    // Attempt to connect
    if (mqttClient.connect(MQTT_CLIENT_ID.c_str(), MQTT_USER, MQTT_PASS, MQTT_TOPIC_STATE, 1, true, MQTT_WILL_MESSAGE, false))
    {
      Serial.println("connected");

      // Once connected, publish an announcement...
      mqttClient.publish(MQTT_TOPIC_STATE, "connected", true);
    }
    else
    {
      Serial.print("failed, rc=");
      Serial.print(mqttClient.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void mqttPublish(const char *topic, float payload)
{
  Serial.print(topic);
  Serial.print(": ");
  Serial.println(payload);

  mqttClient.publish(topic, String(payload).c_str(), true);
}

void setup()
{
  Serial.begin(115200);
  // Serial.println("GY-21_test");

#ifdef ESP8266
  Wire.begin(SDA, SCL);
#else
  Wire.begin();
#endif

  setupWifi();
  mqttClient.setServer(MQTT_SERVER, MQTT_PORT);
}

void loop()
{
  if (!mqttClient.connected())
  {
    mqttReconnect();
  }
  mqttClient.loop();

  long now = millis();
  if (now - lastMsgTime > MQTT_PUBLISH_DELAY)
  {
    lastMsgTime = now;

    float temperature = sensor.GY21_Temperature();
    float humidity = sensor.GY21_Humidity();
    // Publishing sensor data
    mqttPublish(MQTT_TOPIC_TEMPERATURE, temperature);
    mqttPublish(MQTT_TOPIC_HUMIDITY, humidity);
  }
}