#include <ESP8266WiFi.h>
#include <espnow.h>
#include <PubSubClient.h>

String device = "room1";

// Structure example to receive data
// Must match the sender structure
typedef struct struct_message {
  int id;
  int x;
  int y;
} struct_message;

// Change the credentials below, so your ESP8266 connects to your router
const char* ssid = "PyTooL";
const char* password = "123456789";

// Change the variable to your MQTT broker details
const char* mqtt_server = "192.168.137.249";
const int mqtt_port = 1883;
const char* mqtt_user = "root";
const char* mqtt_password = "FIRAS";

// Initializes the espClient
WiFiClient espClient;
PubSubClient client(espClient);

// Create a struct_message called myData
struct_message myData;

// Create a structure to hold the readings from each board
struct_message board1;
struct_message board2;

// Create an array with all the structures
struct_message boardsStruct[2] = {board1, board2};

// Callback function that will be executed when data is received
void OnDataRecv(uint8_t* mac_addr, uint8_t* incomingData, uint8_t len) {
  char macStr[18];
  Serial.print("Packet received from: ");
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
           mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
  Serial.println(macStr);
  memcpy(&myData, incomingData, sizeof(myData));
  Serial.printf("Board ID %u: %u bytes\n", myData.id, len);
  // Update the structures with the new incoming data
  boardsStruct[myData.id - 1].x = myData.x;
  boardsStruct[myData.id - 1].y = myData.y;
  Serial.printf("x value: %d \n", boardsStruct[myData.id - 1].x);
  Serial.printf("y value: %d \n", boardsStruct[myData.id - 1].y);
  Serial.println();
}
WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);
void callback(String 
++, byte* message, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  String messageTemp;

  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  Serial.println();
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP8266Client3", mqtt_user, mqtt_password)) {
      Serial.println("connected");
      // Subscribe or resubscribe to a topic
      // You can subscribe to more topics (to control more LEDs in this example)
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup() {
Serial.begin(115200);

  // Initialize ESP-NOW
  if (esp_now_init() != 0) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Register callback function to handle received ESP-NOW data
  esp_now_register_recv_cb([](uint8_t* mac, uint8_t* data, uint8_t len) {
    Serial.print("Received ESP-NOW data from MAC: ");
    for (int i = 0; i < 6; i++) {
      Serial.print(mac[i], HEX);
      if (i < 5) Serial.print(":");
    }
    Serial.println();

    // Process received ESP-NOW data
    // ...

    // Publish data via MQTT
    mqttClient.publish("/esp8266/dhtreadings", data, len);
  });

  // Initialize Wi-Fi
  WiFi.begin("your_wifi_ssid", "your_wifi_password");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Connecting to Wi-Fi...");
  }
  Serial.println("Connected to Wi-Fi");

  // Initialize MQTT client
  mqttClient.setServer(mqttServer, mqttPort);
  mqttClient.setCallback([](char* topic, byte* payload, unsigned int length) {
    Serial.println("Received MQTT message");
    // Process received MQTT message
    // ...
  });
  
  if (mqttClient.connect(mqttClientId)) {
    Serial.println("Connected to MQTT broker");
    mqttClient.subscribe("/esp8266/dhtreadings");
  }
}
void loop() {
    // Send data using ESP-NOW
  uint8_t espNowData[] = {0x01, 0x02, 0x03};
  esp_now_send(nullptr, espNowData, sizeof(espNowData));

  // Handle MQTT communication
  if (mqttClient.connected()) {
    mqttClient.loop();
  } else {
    if (mqttClient.connect(mqttClientId)) {
      Serial.println("Connected to MQTT broker");
      mqttClient.subscribe("/esp8266/dhtreadings");
    }
  }

  delay(1000);
}
