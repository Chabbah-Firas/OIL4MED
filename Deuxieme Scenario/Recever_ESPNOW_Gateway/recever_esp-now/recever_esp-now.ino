/*
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com/esp-now-many-to-one-esp8266-nodemcu/
  
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files.
  
  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
*/

#include <ESP8266WiFi.h>
#include <espnow.h>
#include <PubSubClient.h>
String device= "room1";
// Structure example to receive data
// Must match the sender structure
typedef struct struct_message {
    int id;
    int temp;
    int humid;
} struct_message;

// Create a struct_message called myData
struct_message myData;

// Create a structure to hold the readings from each board
struct_message board1;
struct_message board2;

// Create an array with all the structures
struct_message boardsStruct[2] = {board1, board2};

// Change the credentials below, so your ESP8266 connects to your router
const char* ssid = "PyTooL";
const char* password = "123456789";

// Change the variable to your Raspberry Pi IP address, so it connects to your MQTT broker
const char* mqtt_server = "192.168.137.249";
const int mqttport= 1883;
const char* mqttUser = "root";
const char* mqttPassword = "FIRAS";


// Initializes the espClient
WiFiClient espClient;
PubSubClient client(espClient);

// Callback function that will be executed when data is received
void OnDataRecv(uint8_t * mac_addr, uint8_t *incomingData, uint8_t len) {
  char macStr[18];
  Serial.print("Packet received from: ");
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
           mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
  Serial.println(macStr);
  memcpy(&myData, incomingData, sizeof(myData));
  Serial.printf("Board ID %u: %u bytes\n", myData.id, len);
  // Update the structures with the new incoming data
  boardsStruct[myData.id-1].temp = myData.temp;
  boardsStruct[myData.id-1].humid = myData.humid;
  Serial.printf("x value: %d \n", boardsStruct[myData.id-1].temp);
  Serial.printf("y value: %d \n", boardsStruct[myData.id-1].humid);
  Serial.println();
}
char data[80];
// Don't change the function below. This functions connects your ESP8266 to your router
void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("WiFi connected - ESP IP address: ");
  Serial.println(WiFi.localIP());
}

// This functions is executed when some device publishes a message to a topic that your ESP8266 is subscribed to
// Change the function below to add logic to your program, so when a device publishes a message to a topic that 
// your ESP8266 is subscribed you can actually do something
void callback(String topic, byte* message, unsigned int length) {
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

// This functions reconnects your ESP8266 to your MQTT broker
// Change the function below if you want to subscribe to more topics with your ESP8266 
void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
     /*
     YOU  NEED TO CHANGE THIS NEXT LINE, IF YOU'RE HAVING PROBLEMS WITH MQTT MULTIPLE CONNECTIONS
     To change the ESP device ID, you will have to give a unique name to the ESP8266.
     Here's how it looks like now:
       if (client.connect("ESP8266Client")) {
     If you want more devices connected to the MQTT broker, you can do it like this:
       if (client.connect("ESPOffice")) {
     Then, for the other ESP:
       if (client.connect("ESPGarage")) {
      That should solve your MQTT multiple connections problem

     THE SECTION IN loop() function should match your device name
    */
    if (client.connect("ESP8266Client3")) {
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
  // Initialize Serial Monitor
  Serial.begin(115200);
  
  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);
  //WiFi.disconnect();
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  // Init ESP-NOW
  if (esp_now_init() != 0) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  
  // Once ESPNow is successfully Init, we will register for recv CB to
  // get recv packer info
  esp_now_set_self_role(ESP_NOW_ROLE_SLAVE);
  esp_now_register_recv_cb(OnDataRecv);
}

void loop(){
  // Access the variables for each board
  int board1X = boardsStruct[0].temp;
  int board1Y = boardsStruct[0].humid;
  int board2X = boardsStruct[1].temp;
  int board2Y = boardsStruct[1].humid;
  String dhtReadings = "{ \"temperature\": \"" + String(board1X) + "\", \"humidity\" : \"" + String(board1Y)+ "\", \"device\" : \"" + device + "\"}";
    dhtReadings.toCharArray(data, (dhtReadings.length() + 1));
    
    // Publishes Temperature and Humidity values
    client.publish("/esp8266/dhtreadings", data);
    Serial.println(data);
    Serial.print("Device : ");
    Serial.print(device);
    Serial.print("Humidity: ");
    Serial.print(board1Y);
    Serial.print(" %\t Temperature: ");
    Serial.print(board1X);
    dhtReadings = "{ \"temperature\": \"" + String(board2X) + "\", \"humidity\" : \"" + String(board2Y)+ "\", \"device\" : \"" + device + "\"}";
    dhtReadings.toCharArray(data, (dhtReadings.length() + 1));
    
    // Publishes Temperature and Humidity values
    client.publish("/esp8266/dhtreadings", data);
    delay(2000);
    Serial.println(data);
    Serial.print("Device : ");
    Serial.print(device);
    Serial.print("Humidity: ");
    Serial.print(board2Y);
    Serial.print(" %\t Temperature: ");
    Serial.print(board2X);
    delay(2000);
}
