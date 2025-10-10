
#include <WiFi.h>
#include <esp_wifi.h>
#include <esp_now.h>

#include "../shared/shared.h"



constexpr int BUTTON_PIN = 12;
const uint8_t RELAY_STATUS_LED = 2;
const uint8_t RELAY_PIN = 22;
const uint8_t CONNECTION_STATUS_PIN = 23;

State current_state = State::OFF;

void on_state_change_request_recv(const esp_now_recv_info* mac, const uint8_t * data, int len)
{
  state_change_request serialized_data;
  memcpy(&serialized_data, data, sizeof(data));

  if (serialized_data.new_state == State::TOGGLE)
  {
    if (current_state == State::ON)
    {
      current_state = State::OFF;
    }
    else
    {
      current_state = State::ON;
    }
  }
  else
  {
    current_state = serialized_data.new_state;
  }
}


void setup() {
 esp_err_t err = esp_wifi_set_mac(WIFI_IF_STA, &base_station_mac_address[0]);
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
  pinMode(CONNECTION_STATUS_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT);

  digitalWrite(CONNECTION_STATUS_PIN, HIGH);
  
  esp_now_register_recv_cb(on_state_change_request_recv);
}

int last_button_state = 0;
int button_state = 0;
uint32_t debounce_time = 0;
constexpr uint32_t DEBOUNCE_DELAY = 100;

void loop() {
  int button_reading = digitalRead(BUTTON_PIN);

  if (button_reading != last_button_state)
  {
    debounce_time = millis();
  }

  if ((millis() - debounce_time) > DEBOUNCE_DELAY)
  {
    // whatever the reading is at, it's been there for longer than the debounce
    // delay, so take it as the actual current state:

    // if the button state has changed:
    if (button_state != button_reading)
    {
      button_state = button_reading;

      // only toggle the LED if the new button state is HIGH
      if (button_state == HIGH)
      {
        if (current_state == State::ON)
        {
          current_state = State::OFF;
        }
        else
        {
          current_state = State::ON;
        }
      }
    }
  }
  last_button_state = button_reading;

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
