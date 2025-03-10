
#include <WiFi.h>
#include <esp_wifi.h>
#include <esp_now.h>



uint8_t newMACAddress[] = {0x01, 0x00, 0x00, 0x00, 0x00, 0x00};
const uint8_t RELAY_STATUS_LED = 23;
const uint8_t RELAY_PIN = 22;

enum class State: uint32_t
{
  OFF = 0x00,
  ON = 0x01,

};

typedef struct state_change_request
{
  State new_state;
} state_change_request;



State current_state = State::OFF;

void on_state_change_request_recv(const esp_now_recv_info* mac, const uint8_t * data, int len)
{
  state_change_request serialized_data;
  memcpy(&serialized_data, data, sizeof(data));

  current_state = serialized_data.new_state;
}


void setup() {
 esp_err_t err = esp_wifi_set_mac(WIFI_IF_STA, &newMACAddress[0]);
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

  pinMode(RELAY_STATUS_LED, OUTPUT);
  pinMode(RELAY_PIN, OUTPUT);
  
  esp_now_register_recv_cb(on_state_change_request_recv);
}

void loop() {
  
  if(current_state == State::ON)
  {
    digitalWrite(RELAY_STATUS_LED, HIGH);
    digitalWrite(RELAY_PIN,HIGH);
  }
  else
  {
    digitalWrite(RELAY_STATUS_LED, LOW);
    digitalWrite(RELAY_PIN,LOW);
  }

}
