#include <WiFi.h>
#include <esp_wifi.h>
#include <esp_now.h>

uint8_t new_mac_address[] = {0x02, 0x00, 0x00, 0x00, 0x00, 0x00};
uint8_t target_mac_address[] = {0x01, 0x00, 0x00, 0x00, 0x00, 0x00};
esp_now_peer_info_t peerInfo;

enum class State: uint32_t
{
  OFF = 0x00,
  ON = 0x01,

};

typedef struct state_change_request
{
  State new_state;
} state_change_request;

void setup() {
  esp_err_t err = esp_wifi_set_mac(WIFI_IF_STA, &new_mac_address[0]);
  if (err == ESP_OK) {
    Serial.println("Successfully changed Mac Address");
  }

  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Register peer
  memcpy(peerInfo.peer_addr, target_mac_address, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
  
  // Add peer        
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }

}

State current_state = State::OFF;

void loop() {
  state_change_request request;
  request.new_state = current_state;
  
  if(current_state == State::ON)
  {
    current_state = State::OFF;
  }
  else
  {
    current_state = State::ON;
  }

  esp_err_t result = esp_now_send(target_mac_address, (uint8_t *) &request, sizeof(request));
   
  if (result == ESP_OK) {
    Serial.println("Sent with success");
  }
  else {
    Serial.println("Error sending the data");
  }
  delay(2000);
}
