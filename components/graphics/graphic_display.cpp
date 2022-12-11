#include "ili9xxx_display.h"
#include "esphome/core/log.h"
#include "esphome/core/application.h"
#include "esphome/core/helpers.h"
#include "esphome/core/hal.h"

namespace esphome {
namespace graphics {

static const char *const TAG = "GraphicsDisplay";

void ILI9XXXDisplay::dump_config() {
  LOG_DISPLAY("", "Arduino_GFX", this);
  LOG_UPDATE_INTERVAL(this);
}


void GraphicsDisplay::setup() {

}

void GraphicsDisplay::update() {
  this->do_update_();
}

int GraphicsDisplay::get_width_internal() { return this->width_; }
int GraphicsDisplay::get_height_internal() { return this->height_; }

void HOT GraphicsDisplay::draw_absolute_pixel_internal(int x, int y, Color color) {
}


}  // namespace graphics
}  // namespace esphome
