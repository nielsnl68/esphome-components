#include "esphome/core/hal.h"
#include "esphome/core/log.h"
#include "AiP31068L_display.h"

namespace esphome {
namespace AiP31068L {

static const char *const TAG = "AiP31068L";

/**
 * @brief 0 = data byte interpreted as command
 *        1 = data byte interpreted as data
 */
static const uint8_t BIT_CONTROL_BYTE_RS = 6;

void AiP31068LLCDDisplay::setup() {
  ESP_LOGCONFIG(TAG, "Setting up AiP31068L LCD Display...");
  LCDDisplay::setup();
}

void AiP31068LLCDDisplay::dump_config() {
  ESP_LOGCONFIG(TAG, "AiP31068L LCD Display:");
  ESP_LOGCONFIG(TAG, "  Columns: %u, Rows: %u", this->columns_, this->rows_);
  LOG_I2C_DEVICE(this);
  LOG_UPDATE_INTERVAL(this);
  if (this->is_failed()) {
    ESP_LOGE(TAG, "Communication with LCD Display failed!");
  }
}


void AiP31068LLCDDisplay::write_n_bits(uint8_t value, uint8_t n) {}

void AiP31068LLCDDisplay::send(uint8_t value, bool rs) {
  uint8_t control_byte = 0;
  if (rs) {
    control_byte |= (1 << BIT_CONTROL_BYTE_RS);
  }
  this->write_bytes(control_byte, &value, 1);
}


} // namespace AiP31068L
} // namespace esphome
