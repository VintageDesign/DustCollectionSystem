#include <WiFi.h>
#include <esp_wifi.h>
#include <esp_now.h>

#include "../shared/shared.h"

esp_now_peer_info_t peerInfo;

constexpr int BUTTON_PIN = 12;
constexpr int STATUS_PIN = 2;


void setup() {
  pinMode(STATUS_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT);

  esp_err_t err = esp_wifi_set_mac(WIFI_IF_STA, &remote_mac_address[0]);
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
  memcpy(peerInfo.peer_addr, base_station_mac_address, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  esp_err_t result = esp_now_add_peer(&peerInfo);
  if (result != ESP_OK) {
    Serial.println("Could Not Find Peer! Error: ");
    Serial.println(esp_err_to_name(result));
    return;
  }

  digitalWrite(STATUS_PIN, HIGH);
}

State current_state = State::OFF;

int last_button_state = 0;
int button_state = 0;
uint32_t debounce_time = 0;
constexpr uint32_t DEBOUNCE_DELAY = 100;

void loop() {

  int button_reading = digitalRead(BUTTON_PIN);


  if (button_reading != last_button_state) {
    debounce_time = millis();
  }

  if ((millis() - debounce_time) > DEBOUNCE_DELAY) {
    // whatever the reading is at, it's been there for longer than the debounce
    // delay, so take it as the actual current state:

    // if the button state has changed:
    if (button_state != button_reading) {
      button_state = button_reading;

      // only toggle the LED if the new button state is HIGH
      if (button_state == HIGH) {
        state_change_request request;
        request.new_state = current_state;

        if (current_state == State::ON) {
          current_state = State::OFF;
        } else {
          current_state = State::ON;
        }

        esp_err_t result = esp_now_send(base_station_mac_address, (uint8_t *)&request, sizeof(request));

        if (result == ESP_OK) {
          Serial.println("Request Sent");
        } else {
          Serial.print("Error Sending Request! Error: ");
          Serial.println(esp_err_to_name(result));
        }
      }
    }
  }
  last_button_state = button_reading;

}
