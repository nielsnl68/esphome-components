#pragma once

#include "esphome/core/component.h"
#include "esphome/components/lcd_base/lcd_display.h"
#include "esphome/components/i2c/i2c.h"

namespace esphome {
namespace AiP31068L {

class AiP31068LLCDDisplay : public lcd_base::LCDDisplay, public i2c::I2CDevice {
 public:
  void set_writer(std::function<void(AiP31068LLCDDisplay &)> &&writer) { this->writer_ = std::move(writer); }
  void setup() override;
  void dump_config() override;
  void backlight();
  void no_backlight();

 protected:
  bool is_four_bit_mode() override { return false; }
  void write_n_bits(uint8_t value, uint8_t n) override;
  void send(uint8_t value, bool rs) override;

  void call_writer() override { this->writer_(*this); }

  // Stores the current state of the backlight.
  uint8_t backlight_value_;
  std::function<void(AiP31068LLCDDisplay &)> writer_;
};

}  // namespace AiP31068L
}  // namespace esphome
