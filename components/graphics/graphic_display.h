#pragma once

#include "esphome/components/display/display_buffer.h"
#include "esphome/components/display/display_color_utils.h"
#include "esphome/components/light/light_state.h"
#include "esphome/core/hal.h"
#include "Arduino_GFX_Library.h"

#ifdef USE_ARDUINO
#define USE_SPI_ARDUINO_BACKEND
#endif

namespace esphome {
namespace graphics {

class GraphicsDisplay;

using gfx_display_writer_t = std::function<void(GraphicsDisplay &)>;

struct Rect {
  static const int16_t RECT_NO_SET = 32766;
  int16_t x;      ///< X coordinate of corner
  int16_t y;      ///< Y coordinate of corner
  int16_t width;  ///< Width of region
  int16_t height; ///< Height of region

  inline Rect() ALWAYS_INLINE : x(RECT_NO_SET),
                                y(RECT_NO_SET),
                                width(RECT_NO_SET),
                                height(RECT_NO_SET) {} // NOLINT
  inline Rect(int16_t x, int16_t y, int16_t width, int16_t height) ALWAYS_INLINE
      : x(x),
        y(y),
        width(width),
        height(height) {}
  inline int16_t x2() {
    return this->x + this->width;
  } ///< X coordinate of corner
  inline int16_t y2() {
    return this->y + this->height;
  } ///< Y coordinate of corner

  inline bool is_set() ALWAYS_INLINE {
    return (this->height != RECT_NO_SET) && (this->width != RECT_NO_SET);
  }

  void expand(int16_t width, int16_t height);
  void join(Rect rect);
  void substract(Rect rect);
  bool inside(int16_t x, int16_t y, bool absolute = false);
  bool inside(Rect rect, bool absolute = false);
  void info(const std::string &prefix = "rect info:");
};

class Clipping {
public:
  ///
  /// Set the clipping rectangle for further drawing
  ///
  /// \param[in]  rect:       Pointer to Rect for clipping (or NULL for entire
  /// screen)
  ///
  /// \return true if success, false if error
  ///
  void push_clipping(Rect rect);
  void push_clipping(int16_t left, int16_t top, int16_t right, int16_t bottom) {
    push_clipping(Rect(left, top, right, bottom));
  };

  ///
  /// Add a rectangular region to the invalidation region
  /// - This is usually called when an element has been modified
  ///
  /// \param[in]  rect: Rectangle to add to the invalidation region
  ///
  /// \return none
  ///
  void add_clipping(Rect rect);
  void add_clipping(int16_t left, int16_t top, int16_t right, int16_t bottom) {
    this->add_clipping(Rect(left, top, right, bottom));
  };

  ///
  /// substract a rectangular region to the invalidation region
  /// - This is usually called when an element has been modified
  ///
  /// \param[in]  rect: Rectangle to add to the invalidation region
  ///
  /// \return none
  ///
  void substract_clipping(Rect rect);
  void substract_clipping(uint16_t left, uint16_t top, uint16_t right,
                          uint16_t bottom) {
    this->substract_clipping(Rect(left, top, right, bottom));
  };

  ///
  /// Reset the invalidation region
  ///
  /// \return none
  ///
  void pop_clipping();

  ///
  /// Get the current the clipping rectangle
  ///
  ///
  /// \return rect for active clipping region
  ///
  Rect get_clipping();

protected:
  std::vector<Rect> clipping_rectangle_;
};

class GraphicsDisplay : public PollingComponent, public Clipping, public Print {
public:
  void dump_config() override;
  void setup() override;
  void update() override;
  float get_setup_priority() const override {
    return setup_priority::HARDWARE;
  };
  display::DisplayType get_display_type() {
    return display::DisplayType::DISPLAY_TYPE_COLOR;
  }
  /// Internal method to set the display writer lambda.
  void set_writer(gfx_display_writer_t &&writer) { this->writer_ = writer; }
  void set_auto_clear(bool auto_clear_enabled) {
    this->auto_clear_enabled_ = auto_clear_enabled;
  }

  void set_Arduino_GFX(Arduino_GFX *gfx, bool use_canvas) { 
    if (use_canvas) {
      gfx = new Arduino_Canvas(gfx->width(), gfx->height(), gfx);
    }
    this->gfx_ = gfx; 
  }

#ifdef USE_LIGHT
  void set_backlight(light::LightState *light) {
    this->light_ = light;

    light->add_new_remote_values_callback([this, light]() {
      // using the remote value because of transitions we need to trigger as
      // early as possible
      auto is_on = light->remote_values.is_on();
      // only trigger when going from off to on
      auto should_trigger = is_on && !this->display_on_;
      // Set new state immediately so that trigger() doesn't devolve
      // into infinite loop
      if (should_trigger) {
        this->display_on();
      }
    });

    light->add_new_target_state_reached_callback([this, light]() {
      auto is_on = light->current_values.is_on();
      // only trigger when going from on to off
      if (!is_on) {
        this->display_off();
      }
    });
  }
#endif
  void display_on() {
    if (this->has_started_) this->gfx_->displayOn();
    this->display_on_ = true;
  }
  void display_off() {
    if (this->has_started_) this->gfx_->displayOff();
    this->display_on_ = false;
  }
  bool is_display_on() { return this->display_on_; }

  void set_rotation(uint8_t rotation) { this->gfx_->setRotation(rotation); }
  uint8_t get_rotation(void) const { return this->gfx_->getRotation(); }

  int16_t get_width(void) const { return this->gfx_->width(); }
  int16_t get_height(void) const { return this->gfx_->height(); }

  void invertDisplay(bool i) { this->gfx_->invertDisplay(i); }

  void set_color(Color color) {
    this->from_color_ = color;
    this->to_color_ = color;
  }
  void set_color(Color from_color, Color to_color) {
    this->from_color_ = from_color;
    this->to_color_ = to_color;
  }
  void set_background_color(Color color) { this->background_color_ = color; }

  void fill(Color color) { this->gfx_->fillScreen(color565(color)); }
  void clear() { this->gfx_->fillScreen(color565(background_color_)); }

  void flush(void) { this->gfx_->flush();}

  void draw_pixel_at(int16_t x, int16_t y) {
    this->gfx_->drawPixel(x, y, color565(from_color_));
  }
  void line(int16_t x0, int16_t y0, int16_t x1, int16_t y1) {
    this->gfx_->drawLine(x0, y0, x1, y1, color565(from_color_));
  }

  void vertical_line(int16_t x, int16_t y, int16_t height) {
    this->gfx_->drawFastVLine(x, y, height, color565(from_color_));
  }
  void horizontal_line(int16_t x, int16_t y, int16_t width) {
    this->gfx_->drawFastHLine(x, y, width, color565(from_color_));
  }

  void rectangle(int16_t x, int16_t y, int16_t width, int16_t height) {
    this->gfx_->drawRect(x, y, width, height, color565(from_color_));
  }
  void rectangle(int16_t x, int16_t y, int16_t width, int16_t height,
                 int16_t radius) {
    this->gfx_->drawRoundRect(x, y, width, height, radius,
                              color565(from_color_));
  }
  void filled_rectangle(int16_t x, int16_t y, int16_t width, int16_t height) {
    this->gfx_->fillRect(x, y, width, height, color565(from_color_));
  }
  void filled_rectangle(int16_t x, int16_t y, int16_t width, int16_t height,
                        int16_t radius) {
    this->gfx_->fillRoundRect(x, y, width, height, radius,
                              color565(from_color_));
  }

  void circle(int16_t x, int16_t y, int16_t radius) {
    this->gfx_->drawCircle(x, y, radius, color565(from_color_));
  }
  void filled_circle(int16_t x, int16_t y, int16_t radius) {
    this->gfx_->fillCircle(x, y, radius, color565(from_color_));
  }

  void triangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2,
                int16_t y2) {
    this->gfx_->drawTriangle(x0, y0, x1, y1, x2, y2, color565(from_color_));
  }
  void filled_triangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1,
                       int16_t x2, int16_t y2) {
    this->gfx_->fillTriangle(x0, y0, x1, y1, x2, y2, color565(from_color_));
  }

  void ellipse(int16_t x, int16_t y, int16_t radius_x, int16_t radius_y) {
    this->gfx_->drawEllipse(x, y, radius_x, radius_y, color565(from_color_));
  }
  void ellipse_helper(int32_t x, int32_t y, int32_t radius_x, int32_t radius_y,
                      uint8_t corner) {
    this->gfx_->drawEllipseHelper(x, y, radius_x, radius_y, corner,
                                  color565(from_color_));
  }
  void filled_ellipse(int16_t x, int16_t y, int16_t radius_x,
                      int16_t radius_y) {
    this->gfx_->fillEllipse(x, y, radius_x, radius_y, color565(from_color_));
  }
  void filled_ellipse_helper(int32_t x, int32_t y, int32_t radius_x,
                             int32_t radius_y, uint8_t corner, int16_t delta) {
    this->gfx_->fillEllipseHelper(x, y, radius_x, radius_y, corner, delta,
                                  color565(from_color_));
  }
  void arc(int16_t x, int16_t y, int16_t radius_outer, int16_t radius_inner,
           float start, float end) {
    this->gfx_->drawArc(x, y, radius_outer, radius_inner, start, end,
                        color565(from_color_));
  }
  void filled_arc(int16_t x, int16_t y, int16_t radius_outer,
                  int16_t radius_inner, float start, float end) {
    this->gfx_->fillArc(x, y, radius_outer, radius_inner, start, end,
                        color565(from_color_));
  }
  void filled_arc_helper(int16_t cx, int16_t cy, int16_t radius_outer,
                         int16_t radius_inner, float start, float end) {
    this->gfx_->fillArcHelper(cx, cy, radius_outer, radius_inner, start, end,
                              color565(from_color_));
  }

  void transparant_image(int16_t x, int16_t y, int16_t width, int16_t height,
                         const uint8_t bitmap[]) {
    this->gfx_->drawBitmap(x, y, bitmap, width, height, color565(from_color_));
  }
  void transparant_image(int16_t x, int16_t y, int16_t width, int16_t height,
                         uint8_t *bitmap) {
    this->gfx_->drawBitmap(x, y, bitmap, width, height, color565(from_color_));
  }
  void image(int16_t x, int16_t y, int16_t width, int16_t height,
             const uint8_t bitmap[]) {
    this->gfx_->drawBitmap(x, y, bitmap, width, height, color565(from_color_),
                           color565(background_color_));
  }
  void image(int16_t x, int16_t y, int16_t width, int16_t height,
             uint8_t *bitmap) {
    this->gfx_->drawBitmap(x, y, bitmap, width, height, color565(from_color_),
                           color565(background_color_));
  }
  void xbm_image(int16_t x, int16_t y, int16_t width, int16_t height,
                 const uint8_t bitmap[]) {
    this->gfx_->drawXBitmap(x, y, bitmap, width, height, color565(from_color_));
  }

  void indexed_image(int16_t x, int16_t y, int16_t width, int16_t height,
                     uint8_t *bitmap, uint16_t *color_index) {
    this->gfx_->drawIndexedBitmap(x, y, bitmap, color_index, width, height);
  }
  //  void 3bit_image(int16_t x, int16_t y, int16_t width, int16_t height,
  //  uint8_t *bitmap) {
  //    this->gfx_->draw3bitRGBBitmap(x, y, bitmap, width, height);
  //  }

  void get_text_bounds(const char *string, int16_t x, int16_t y, int16_t *x1,
                       int16_t *y1, uint16_t *width, uint16_t *height) {
    this->gfx_->getTextBounds(string, x, y, x1, y1, width, height);
  }
  void get_text_bounds(const __FlashStringHelper *string, int16_t x, int16_t y,
                       int16_t *x1, int16_t *y1, uint16_t *width,
                       uint16_t *height) {
    this->gfx_->getTextBounds(string, x, y, x1, y1, width, height);
  }
  void get_text_bounds(const String &string, int16_t x, int16_t y, int16_t *x1,
                       int16_t *y1, uint16_t *width, uint16_t *height) {
    this->gfx_->getTextBounds(string, x, y, x1, y1, width, height);
  }

  void set_text_size(uint8_t s) { this->gfx_->setTextSize(s); }
  void set_text_size(uint8_t sx, uint8_t sy) {
    this->gfx_->setTextSize(sx, sy);
  }
  void set_text_size(uint8_t sx, uint8_t sy, uint8_t pixel_margin) {
    this->gfx_->setTextSize(sx, sy, pixel_margin);
  }

  void set_font(const GFXfont *f = NULL) { this->gfx_->setFont(f); }

  void draw_char(int16_t x, int16_t y, unsigned char c, Color color, Color bg) {
    this->gfx_->drawChar(x, y, c, color565(color), color565(bg));
  }

  void set_cursor(int16_t x, int16_t y) { this->gfx_->setCursor(x, y); }
  int16_t get_cursor_x(void) const { return this->gfx_->getCursorX(); }
  int16_t get_cursor_y(void) const { return this->gfx_->getCursorY(); };

  void set_text_color(int16_t c) { this->gfx_->setTextColor(c); }
  void set_text_color(int16_t c, int16_t bg) {
    this->gfx_->setTextColor(c, bg);
  }

  void set_text_color(Color c) { this->gfx_->setTextColor(color565(c)); }
  void set_text_color(Color c, Color bg) {
    this->gfx_->setTextColor(color565(c), color565(bg));
  }

  void set_text_wrap(bool wrap) { this->gfx_->setTextWrap(wrap); }

  size_t write(uint8_t chr) { return this->gfx_->write(chr); }

  uint16_t color565(Color clr) {
    return ((clr.red & 0xF8) << 8) | ((clr.green & 0xFC) << 3) |
           (clr.blue >> 3);
  }

  uint16_t color565(uint8_t red, uint8_t green, uint8_t blue) {
    return ((red & 0xF8) << 8) | ((green & 0xFC) << 3) | (blue >> 3);
  }

protected:
  Arduino_GFX *gfx_{nullptr};
  bool auto_clear_enabled_{true};
  optional<gfx_display_writer_t> writer_{};
#ifdef USE_LIGHT
  light::LightState *light_{};
#endif
  Color from_color_{Color::WHITE};
  Color to_color_ = {Color::WHITE};
  Color background_color_{Color::BLACK};
  bool display_on_{true};
  bool has_started_{false};
};

} // namespace graphics
} // namespace esphome
