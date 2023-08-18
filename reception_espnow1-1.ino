#include <ESP8266WiFi.h>
#include <espnow.h>

// Structure example to receive data
// Must match the sender structure

typedef struct struct_message {
  char a[32];
  float temp;
  float humid;
  String room;
  bool e;
} struct_message;

// Create a struct_message called myData
struct_message myData;

// Callback function that will be executed when data is received
void OnDataRecv(uint8_t * mac, uint8_t *incomingData, uint8_t len) {
  memcpy(&myData, incomingData, sizeof(myData));
  Serial.print("Bytes received: ");
  Serial.print(len);
  Serial.println(" Bytes .");
  Serial.print("Nom d'agricultaire : ");
  Serial.println(myData.a);
  Serial.print("Le numéro de chambre :");
  Serial.println(myData.room);
  Serial.print("La température : ");
  Serial.print(myData.temp);
  Serial.println(" C° .");
  Serial.print("L'humidité :  ");
  Serial.print(myData.humid);
  Serial.println(" % .");
  Serial.print("Verification de donnée de capteur :  ");
  Serial.println(myData.e);
  Serial.println();
}
 
void setup() {
  // Initialize Serial Monitor
  Serial.begin(115200);
  
  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

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

void loop() {
  
}
