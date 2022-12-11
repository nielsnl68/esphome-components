#include "graphic_display.h"
#include "esphome/core/log.h"
#include "esphome/core/application.h"
#include "esphome/core/helpers.h"
#include "esphome/core/hal.h"

namespace esphome {
namespace graphics {

static const char *const TAG = "GraphicsDisplay";

void GraphicsDisplay::dump_config() {
  LOG_DISPLAY("", "Arduino_GFX", this);
  LOG_UPDATE_INTERVAL(this);
}


void GraphicsDisplay::setup() {

}

void GraphicsDisplay::update() {
  this->do_update_();
}

}  // namespace graphics
}  // namespace esphome
