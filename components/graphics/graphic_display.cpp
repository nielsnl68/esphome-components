#include "graphic_display.h"
#include "esphome/core/application.h"
#include "esphome/core/hal.h"
#include "esphome/core/helpers.h"
#include "esphome/core/log.h"

namespace esphome {
namespace graphics {

static const char *const TAG = "GraphicsDisplay";

void Rect::expand(int16_t width, int16_t height) {
  if ((*this).is_set() && ((*this).width >= (-2 * width)) &&
      ((*this).height >= (-2 * height))) {
    (*this).x = (*this).x - width;
    (*this).y = (*this).y - height;
    (*this).width = (*this).width + (2 * width);
    (*this).height = (*this).height + (2 * height);
  }
}

void Rect::join(Rect rect) {
  if (!this->is_set()) {
    this->x = rect.x;
    this->y = rect.y;
    this->width = rect.width;
    this->height = rect.height;
  } else {
    if (this->x > rect.x) {
      this->x = rect.x;
    }
    if (this->y > rect.y) {
      this->y = rect.y;
    }
    if (this->x2() < rect.x2()) {
      this->width = rect.x2() - this->x;
    }
    if (this->y2() < rect.y2()) {
      this->height = rect.y2() - this->y;
    }
  }
}
void Rect::substract(Rect rect) {
  if (!this->inside(rect)) {
    (*this) = Rect();
  } else {
    if (this->x < rect.x) {
      this->x = rect.x;
    }
    if (this->y < rect.y) {
      this->y = rect.y;
    }
    if (this->x2() > rect.x2()) {
      this->width = rect.x2() - this->x;
    }
    if (this->y2() > rect.y2()) {
      this->height = rect.y2() - this->y;
    }
  }
}
bool Rect::inside(int16_t x, int16_t y, bool absolute) {
  if (!this->is_set()) {
    return true;
  }
  if (absolute) {
    return ((x >= 0) && (x <= this->width) && (y >= 0) && (y <= this->height));
  } else {
    return ((x >= this->x) && (x <= this->x2()) && (y >= this->y) &&
            (y <= this->y2()));
  }
}
bool Rect::inside(Rect rect, bool absolute) {
  if (!this->is_set() || !rect.is_set()) {
    return true;
  }
  if (absolute) {
    return ((rect.x <= this->width) && (rect.width >= 0) &&
            (rect.y <= this->height) && (rect.height >= 0));
  } else {
    return ((rect.x <= this->x2()) && (rect.x2() >= this->x) &&
            (rect.y <= this->y2()) && (rect.y2() >= this->y));
  }
}
void Rect::info(const std::string &prefix) {
  if (this->is_set()) {
    ESP_LOGI(TAG, "%s [%3d,%3d,%3d,%3d]", prefix.c_str(), this->x, this->y,
             this->width, this->height);
  } else
    ESP_LOGI(TAG, "%s ** IS NOT SET **", prefix.c_str());
}

void Clipping::push_clipping(Rect rect) {
  // ESP_LOGW(TAG, "set: Push new clipping");
  if (!this->clipping_rectangle_.empty()) {
    Rect r = this->clipping_rectangle_.back();
    rect.substract(r);
  }
  this->clipping_rectangle_.push_back(rect);
}
void Clipping::pop_clipping() {
  if (this->clipping_rectangle_.empty()) {
    ESP_LOGE(TAG, "clear: Clipping is not set.");
  } else {
    this->clipping_rectangle_.pop_back();
  }
}
void Clipping::add_clipping(Rect add_rect) {
  if (this->clipping_rectangle_.empty()) {
    ESP_LOGE(TAG, "add: Clipping is not set.");
  } else {
    this->clipping_rectangle_.back().join(add_rect);
  }
}
void Clipping::substract_clipping(Rect add_rect) {
  if (this->clipping_rectangle_.empty()) {
    ESP_LOGE(TAG, "add: Clipping is not set.");
  } else {
    // ESP_LOGW(TAG, "add: join new clipping");
    this->clipping_rectangle_.back().substract(add_rect);
  }
}
Rect Clipping::get_clipping() {
  if (this->clipping_rectangle_.empty()) {
    return Rect();
  } else {
    return this->clipping_rectangle_.back();
  }
}

void GraphicsDisplay::dump_config() {
  ESP_LOGCONFIG(TAG, "Display Graphics");
  ESP_LOGCONFIG(TAG, "  Rotations: %d °", (this)->get_rotation());
  ESP_LOGCONFIG(TAG, "  Dimensions: %dpx x %dpx", (this)->get_width(),
                (this)->get_height());
  LOG_UPDATE_INTERVAL(this);
}

void GraphicsDisplay::setup() {
  this->gfx_->begin();
  this->clear();
  this->has_started_ = true;

#ifdef USE_LIGHT
  if (!this->display_on_ ||
      (light_ != nullptr && !this->light_->current_values.is_on())) {
#else
  if (!this->display_on_) {
#endif
    display_off();
  }
}

void GraphicsDisplay::update() {
  static bool prossing_update = false, need_update = false;
  if (prossing_update) {
    need_update = true;
    return;
  }
  if (this->auto_clear_enabled_) {
    this->clear();
  }
  do {
    prossing_update = true;
    need_update = false;
    if (!need_update && this->writer_.has_value()) {
      (*this->writer_)(*this);
    }
  } while (need_update);
  flush();
  prossing_update = false;
}

} // namespace graphics
} // namespace esphome
