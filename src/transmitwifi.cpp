
#include <WiFi.h>
#include "transmitwifi.h"
#include "SPIFFS.h"

File file;
void mqttCallback(char *topic, byte *message, unsigned int length)
{
  delay(2000);
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
    if (messageTemp == "FIRE")
    {
      debugln("on");
      digitalWrite(EJECTION_PIN, HIGH);
      delay(2000);
    }
    else if (messageTemp == "ABORT")
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
  digitalWrite(LED_BUILTIN, HIGH);
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
  digitalWrite(LED_BUILTIN, LOW);

  client.setBufferSize(MQTT_BUFFER_SIZE);
  client.setServer(mqtt_server, MQQT_PORT);
  client.setCallback(mqttCallback);
  client.subscribe("controls/ejection");
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
    sprintf(mqttMessage,
            "%lld,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.2f,%.16f,%.16f,%i\n",
            sv.timeStamp,//0
            sv.ax,//1
            sv.ay,//2
            sv.az,//3
            sv.gx,//4
            sv.gy,//5
            sv.gz,//6
            sv.altitude,//7
            sv.filtered_s,//8
            sv.filtered_v,//9
            sv.filtered_a,//10
            sv.latitude,//11
            sv.longitude,//12
            sv.state//13
        );
    file = SPIFFS.open("/log.csv", FILE_APPEND);
    client.publish("n3/telemetry", mqttMessage);
    if (file.print(mqttMessage))
    {
      debugln("[+] Message appended");
    }
    else
    {
      debugln("[-] Append failed");
    }
    file.close();
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