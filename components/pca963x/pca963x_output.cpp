#include "pca963x_output.h"
#include "esphome/core/helpers.h"
#include "esphome/core/log.h"

namespace esphome {
namespace pca963x {

static const char *TAG = "pca963x";

static const uint8_t PCA963X_MODE1_SLEEP = 0x10;

static const uint8_t PCA963X_MODE2_BLINK = 0x20;
static const uint8_t PCA963X_MODE2_INVERTED = 0x10;
static const uint8_t PCA963X_MODE2_OUTPUT_ONACK = 0x08;
static const uint8_t PCA963X_MODE2_OUTPUT_TOTEM_POLE = 0x04;
static const uint8_t PCA963X_MODE2_OUTNE_DEFAULT = 0x01;

static const uint8_t PCA963X_REGISTER_MODE1 = 0x00;
static const uint8_t PCA963X_REGISTER_MODE2 = 0x01;
static const uint8_t PCA963X_REGISTER_LED0 = 0x02;
static const uint8_t PCA963X_REGISTER_LEDOUT = 0x04; // + number of channels

void PCA963xOutput::setup() {
  ESP_LOGCONFIG(TAG, "Setting up PCA963xOutput Component...");

  // clear/ reset registers
  this->write_byte(PCA963X_REGISTER_MODE1, 0x00);
  this->write_byte(PCA963X_REGISTER_MODE2, 0x00);
  this->write_byte(PCA963X_REGISTER_LEDOUT + this->max_channels_, 0xff);
  if (this->max_channels_ > 4) {
    this->write_byte(PCA963X_REGISTER_LEDOUT + this->max_channels_ + 1, 0xff);
  }
  if (this->max_channels_ > 8) {
    this->write_byte(PCA963X_REGISTER_LEDOUT + this->max_channels_ + 2, 0xff);
  }
}

void PCA963xOutput::dump_config() {
  ESP_LOGCONFIG(TAG, "PCA963X: 0x%X", (0x7c >> 1));
  // ESP_LOGCONFIG(TAG, "  Mode: 0x%02X", this->mode_);
  
  LOG_I2C_DEVICE(this);

  if (this->is_failed()) {
    ESP_LOGE(TAG, "Setting up PCA963X failed!");
  }
}

void PCA963xOutput::wakeUp() {

  uint8_t prevReg;
  if (this->read_byte(PCA963X_REGISTER_MODE1, &prevReg)) {
    uint8_t newReg = prevReg & ~(PCA963X_MODE1_SLEEP);

    this->write_byte(PCA963X_REGISTER_MODE1, newReg);
  }
}

void PCA963xOutput::sleep() {

  uint8_t prevReg;
  if (this->read_byte(PCA963X_REGISTER_MODE1, &prevReg)) {
    uint8_t newReg = prevReg | (PCA963X_MODE1_SLEEP);

    this->write_byte(PCA963X_REGISTER_MODE1, newReg);
  }
}

void PCA963xOutput::loop() {
  if (!this->update_)
    return;
  uint8_t prevReg;
  if (this->read_byte(PCA963X_REGISTER_MODE2, &prevReg)) {
    uint8_t newReg = prevReg & ~(PCA963X_MODE2_BLINK | PCA963X_MODE2_INVERTED |
                                 PCA963X_MODE2_OUTPUT_TOTEM_POLE);
    if (this->blink_mode_)
      newReg = newReg | PCA963X_MODE2_BLINK;
    if (this->blink_mode_)
      newReg = newReg | PCA963X_MODE2_OUTPUT_TOTEM_POLE;
    if (this->blink_mode_)
      newReg = newReg | PCA963X_MODE2_INVERTED;
    if (prevReg != newReg ) {
      if (!this->write_byte(PCA963X_REGISTER_MODE2, newReg)) {
        ESP_LOGE(TAG, "Cant write config: 0x%X to 0x%X !", newReg, PCA963X_REGISTER_MODE2);
        this->status_set_warning();
        return;
      }
      //delay(100);
    }
  } else {
    ESP_LOGE(TAG, "Cant read config: %d !", PCA963X_REGISTER_MODE2);
    this->status_set_warning();
    return;
  }

  for (uint8_t channel = 0; channel < this->max_channels_; channel++) {
    uint8_t reg = PCA963X_REGISTER_LED0 + channel;
      ESP_LOGD(TAG, "Write value: 0x%X to 0x%X of %d!", pwm_values_[channel],  reg, this->max_channels_);
      if (!this->write_bytes(reg, &pwm_values_[channel], 1)) {
      ESP_LOGE(TAG, "Cant write value: 0x%X to 0x%X !", pwm_values_[channel],  reg);
      this->status_set_warning();
      return;
    }
  }

  this->status_clear_warning();
  this->update_ = false;
}

PCA963xChannel *PCA963xOutput::create_channel(uint8_t channel) {
  if (channel >= this->max_channels_) {
    ESP_LOGE(TAG, "You cant use channel: %d !", channel);
    this->mark_failed();
    return nullptr;
  }
  return new PCA963xChannel(this, channel);
}

void PCA963xChannel::write_state(float state) {
  uint8_t value = static_cast<uint8_t>(roundf(state * 255));
  this->parent_->set_channel_value_(this->channel_, value);
}

} // namespace pca963x
} // namespace esphome
