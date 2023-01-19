
#include <WiFi.h>
#include "transmitwifi.h"

void mqttCallback(char *topic, byte *message, unsigned int length)
{
  debug("Message arrived on topic: ");
  debug(topic);
  debug(". Message: ");
  String messageTemp;

  for (int i = 0; i < length; i++)
  {
    debug((char)message[i]);
    messageTemp += (char)message[i];
  }
  debugln();

  // Feel free to add more if statements to control more GPIOs with MQTT

  // If a message is received on the topic esp32/output, you check if the message is either "on" or "off".
  // Changes the output state according to the message
  if (String(topic) == "controls/ejection")
  {
    debug("Changing output to ");
    if (messageTemp == "on")
    {
      debugln("on");
      digitalWrite(EJECTION_PIN, HIGH);
    }
    else if (messageTemp == "off")
    {
      debugln("off");
      digitalWrite(EJECTION_PIN, LOW);
    }
  }
}
//if you plan on using the NodeMCU as the access point for wifi access
void create_Accesspoint()
{
debugln();
debug("creating access point ");
debugln("ssid: ");
debugln(ssid);
debugln("password: ");
debugln(password);
 WiFi.softAP(ssid, password);
  IPAddress IP = WiFi.softAPIP();
  debugln("Access point successully created ");
  debugln("IP address: ");
  debugln(IP);
  client.setBufferSize(MQTT_BUFFER_SIZE);
  client.setServer(mqtt_server, MQQT_PORT);
  client.setCallback(mqttCallback);

}

//If you plan on connecting to an external access point as with the case of the outdoor unit
void setup_wifi()
{
  // Connect to a WiFi network
  debugln();
  debug("Connecting to ");
  debugln(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    debug(".");
    
  }

  debugln("");
  debugln("WiFi connected");
  debugln("IP address: ");
  debugln(WiFi.localIP());

  client.setBufferSize(MQTT_BUFFER_SIZE);
  client.setServer(mqtt_server, MQQT_PORT);
  client.setCallback(mqttCallback);
}

void reconnect()
{
  // Loop until we're reconnected
  while (!client.connected())
  {
    debugln("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP8266Client"))
    {
      debugln("connected to Mosquitto server");
      // Subscribe
      client.subscribe("controls/ejection");
    }
    else
    {
      debug("failed, rc=");
      debug(client.state());
      debugln(" try again in 50 milliseconds");
      // Wait 5 seconds before retrying
      delay(50);
    }
  }
}

void sendTelemetryWiFi(Data sv)
{

    // publish whole message i json
    char mqttMessage[300];
    sprintf(mqttMessage, "{\"timestamp\":%lld,\"altitude\":%.3f,\"temperature\":%.3f,\"ax\":%.3f,\"ay\":%.3f,\"az\":%.3f,\"gx\":%.3f,\"gy\":%.3f,\"gz\":%.3f,\"filtered_s\":%.3f,\"filtered_v\":%.3f,\"filtered_a\":%.3f,\"state\":%d,\"longitude\":%.8f,\"latitude\":%.8f}", sv.timeStamp, sv.altitude,sv.temperature,sv.ax,sv.ay,sv.az,sv.gx,sv.gy,sv.gz,sv.filtered_s,sv.filtered_v,sv.filtered_a, sv.state, sv.longitude, sv.latitude);
    client.publish("esp32/message", mqttMessage);
    debugln(mqttMessage);
}

void handleWiFi(Data sv)
{
  if (!client.connected())
  {
    reconnect();
  }
  client.loop();
  sendTelemetryWiFi(sv);
}