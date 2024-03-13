#include <ESP8266WiFi.h>
#include <PubSubClient.h>

const int MAX_LENGTH = 64;
const char* SSID = "Alt-F4";
const char* PASSWORD = "12345678";
const char* mqttServer = "192.168.3.84";
const char* pubTopic = "Sensor-Data";
const char* subTopic = "Car-Control";
const char* clientID = "esp8266-car";

int pubStatus = 0;
int subStatus = 0;
int mqttStatus = 0;
char charBuffer;
char msgBuffer[MAX_LENGTH];
int i = 0;

#define LOG_DEBUG

WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

void connectMQTTServer() {
  mqttStatus = mqttClient.connect(clientID);
  if (mqttStatus) {
    subscribeTopic();
  } else {
    delay(3000);
  }
#ifdef LOG_DEBUG
  if (mqttStatus) {
    Serial.println("MQTT Server Connected.");
    Serial.println("Server Address: ");
    Serial.println(mqttServer);
    Serial.println("ClientID:");
    Serial.println(clientID);
  } else {
    Serial.print("MQTT Server Connect Failed. Client State:");
    Serial.println(mqttClient.state());
    delay(3000);
  }
#endif
}

void pubMQTTmsg() {
  pubStatus = mqttClient.publish(pubTopic, msgBuffer);
#ifdef LOG_DEBUG
  if (pubStatus) {
    Serial.println("Publish Topic:");
    Serial.println(pubTopic);
    Serial.println("Publish message:");
    Serial.println(msgBuffer);
  } else {
    Serial.println("Message Publishng Failed.");
  }
#endif
}

void subscribeTopic() {
  subStatus = mqttClient.subscribe(subTopic);
#ifdef LOG_DEBUG
  if (subStatus) {
    Serial.println("Subscrib Topic:");
    Serial.println(subTopic);
  } else {
    Serial.println("Subscribe Fail...");
  }
#endif
}

void receiveCallback(char* topic, byte* payload, unsigned int length) {
#ifdef LOG_DEBUG
  Serial.print("Message Received [");
  Serial.print(topic);
  Serial.print("] ");
  Serial.print("Message Length(Bytes) ");
  Serial.println(length);
#endif
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

void connectWifi() {
  WiFi.begin(SSID, PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
  }
#ifdef LOG_DEBUG
  Serial.println("WiFi Connected!");
#endif
}

void readMessage() {
  while (Serial.available() > 0) {
    charBuffer = Serial.read();
    if (charBuffer != '\n' && (i < MAX_LENGTH - 1)) {
      msgBuffer[i++] = charBuffer;
    } else {
      msgBuffer[i] = '\0';
      i = 0;
      pubMQTTmsg();
    }
  }
}

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.setAutoReconnect(true);
  WiFi.persistent(true);
  connectWifi();
  mqttClient.setServer(mqttServer, 1883);
  mqttClient.setCallback(receiveCallback);
  connectMQTTServer();
}

void loop() {
  if (mqttClient.connected()) {
    readMessage();
    mqttClient.loop();
  } else {
    connectMQTTServer();
  }
}
