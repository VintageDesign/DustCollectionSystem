#pragma once

uint8_t remote_mac_address[] = {0x02, 0x00, 0x00, 0x00, 0x00, 0x00};
uint8_t base_station_mac_address[] = {0x01, 0x00, 0x00, 0x00, 0x00, 0x00};

enum class State: uint32_t
{
  OFF = 0x00,
  ON = 0x01,
  TOGGLE = 0x02, // Invert the current state.
};

typedef struct state_change_request
{
  State new_state;
} state_change_request;
