#pragma once

#include "esphome/components/display/display_buffer.h"
#include "esphome/core/hal.h"
#include "Arduino_GFX_Library.h"

#ifdef USE_ARDUINO
#define USE_SPI_ARDUINO_BACKEND
#endif

#ifdef USE_SPI_ARDUINO_BACKEND
#include <SPI.h>
#endif

namespace esphome {
namespace graphics {


struct Rect {
  static const int16_t RECT_NO_SET = 32766;
  int16_t x;  ///< X coordinate of corner
  int16_t y;  ///< Y coordinate of corner
  int16_t w;  ///< Width of region
  int16_t h;  ///< Height of region

  inline Rect() ALWAYS_INLINE : x(RECT_NO_SET), y(RECT_NO_SET), w(RECT_NO_SET), h(RECT_NO_SET) {}  // NOLINT
  inline Rect(int16_t x, int16_t y, int16_t w, int16_t h) ALWAYS_INLINE : x(x), y(y), w(w), h(h) {}
  inline int16_t x2() { return this->x + this->w; }  ///< X coordinate of corner
  inline int16_t y2() { return this->y + this->h; }  ///< Y coordinate of corner

  inline bool is_set() ALWAYS_INLINE { return (this->h != RECT_NO_SET) && (this->w != RECT_NO_SET); }

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
  /// \param[in]  rect:       Pointer to Rect for clipping (or NULL for entire screen)
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
  void substract_clipping(uint16_t left, uint16_t top, uint16_t right, uint16_t bottom) {
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


class GraphicsDisplay : public display::DisplayBuffer, public PollingComponent, public Clipping, public Print {
public:
  void dump_config() override;
  void setup() override;
  void update() override;
  float get_setup_priority() const override {
    return setup_priority::HARDWARE;
  };
  display::DisplayType get_display_type() override {
    return display::DisplayType::DISPLAY_TYPE_COLOR;
  }

  void set_Arduino_GFX(Arduino_GFX *gfx) { this->gfx_ = gfx; }

  // TRANSACTION API / CORE DRAW API
  // These MAY be overridden by the subclass to provide device-specific
  // optimized code.  Otherwise 'generic' versions are used.
  void startWrite() { this->gfx_->startWrite(); }
  void writeFillRectPreclipped(int16_t x, int16_t y, int16_t w, int16_t h,
                               uint16_t color) {
    this->gfx_->writeFillRectPreclipped(x, y, w, h, color);
  }
  void writeFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color) {
    this->gfx_->writeFastVLine(x, y, h, color);
  }
  void writeFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color) {
    this->gfx_->writeFastHLine(x, y, w, color);
  }
  void writeLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1,
                 uint16_t color) {
    this->gfx_->writeLine(x0, y0, x1, y1, color);
  }
  INLINE void endWrite(void) { this->gfx_->endWrite(); }

  void setRotation(uint8_t rotation) { this->gfx_->setRotation(rotation); }
  void invertDisplay(bool i) { this->gfx_->invertDisplay(i); }
  void displayOn() { this->gfx_->displayOn(); }
  void displayOff() { this->gfx_->displayOff(); }


  void writePixel(int16_t x, int16_t y, uint16_t color) {
    this->gfx_->writePixel(x, y, color);
  }
  void drawPixel(int16_t x, int16_t y, uint16_t color) {
    this->gfx_->drawPixel(x, y, color);
  }
  void drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color) {
    this->gfx_->drawFastVLine(x, y, h, color);
  }
  void drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color) {
    this->gfx_->drawFastHLine(x, y, w, color);
  }
  void writeFillRect(int16_t x, int16_t y, int16_t w, int16_t h,
                     uint16_t color) {
    this->gfx_->writeFillRect(x, y, w, h, color);
  }
  void fillScreen(uint16_t color) { this->gfx_->fillScreen(color); }
  void drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1,
                uint16_t color) {
    this->gfx_->drawLine(x0, y0, x1, y1, color);
  }
  void drawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) {
    this->gfx_->drawRect(x, y, w, h, color);
  }
  void fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) {
    this->gfx_->fillRect(x, y, w, h, color);
  }
  void drawCircle(int16_t x0, int16_t y0, int16_t radius, uint16_t color) {
    this->gfx_->drawCircle(x0, y0, radius, color);
  }
  void fillCircle(int16_t x0, int16_t y0, int16_t radius, uint16_t color) {
    this->gfx_->fillCircle(x0, y0, radius, color);
  }
  void drawTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2,
                    int16_t y2, uint16_t color) {
    this->gfx_->drawTriangle(x0, y0, x1, y1, x2, y2, color);
  }
  void fillTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2,
                    int16_t y2, uint16_t color) {
    this->gfx_->fillTriangle(x0, y0, x1, y1, x2, y2, color);
  }
  void drawRoundRect(int16_t x0, int16_t y0, int16_t w, int16_t h,
                     int16_t radius, uint16_t color) {
    this->gfx_->drawRoundRect(x0, y0, w, h, radius, color);
  }
  void fillRoundRect(int16_t x0, int16_t y0, int16_t w, int16_t h,
                     int16_t radius, uint16_t color) {
    this->gfx_->fillRoundRect(x0, y0, w, h, radius, color);
  }
  void drawEllipse(int16_t x, int16_t y, int16_t rx, int16_t ry,
                   uint16_t color) {
    this->gfx_->drawEllipse(x, y, rx, ry, color);
  }
  void drawEllipseHelper(int32_t x, int32_t y, int32_t rx, int32_t ry,
                         uint8_t cornername, uint16_t color) {
    this->gfx_->drawEllipseHelper(x, y, rx, ry, cornername, color);
  }
  void fillEllipse(int16_t x, int16_t y, int16_t rx, int16_t ry,
                   uint16_t color) {
    this->gfx_->fillEllipse(x, y, rx, ry, color);
  }
  void fillEllipseHelper(int32_t x, int32_t y, int32_t rx, int32_t ry,
                         uint8_t cornername, int16_t delta, uint16_t color) {
    this->gfx_->fillEllipseHelper(x, y, rx, ry, cornername, delta, color);
  }
  void drawArc(int16_t x, int16_t y, int16_t r1, int16_t r2, float start,
               float end, uint16_t color) {
    this->gfx_->drawArc(x, y, r1, r2, start, end, color);
  }
  void fillArc(int16_t x, int16_t y, int16_t r1, int16_t r2, float start,
               float end, uint16_t color) {
    this->gfx_->fillArc(x, y, r1, r2, start, end, color);
  }
  void fillArcHelper(int16_t cx, int16_t cy, int16_t oradius, int16_t iradius,
                     float start, float end, uint16_t color) {
    this->gfx_->fillArcHelper(cx, cy, oradius, iradius, start, end, color);
  }

  void drawBitmap(int16_t x, int16_t y, const uint8_t bitmap[], int16_t w,
                  int16_t h, uint16_t color) {
    this->gfx_->drawBitmap(x, y, bitmap, w, h, color);
  }
  void drawBitmap(int16_t x, int16_t y, uint8_t *bitmap, int16_t w, int16_t h,
                  uint16_t color) {
    this->gfx_->drawBitmap(x, y, bitmap, w, h, color);
  }
  void drawBitmap(int16_t x, int16_t y, const uint8_t bitmap[], int16_t w,
                  int16_t h, uint16_t color, uint16_t bg) {
    this->gfx_->drawBitmap(x, y, bitmap, w, h, color, bg);
  }
  void drawBitmap(int16_t x, int16_t y, uint8_t *bitmap, int16_t w, int16_t h,
                  uint16_t color, uint16_t bg) {
    this->gfx_->drawBitmap(x, y, bitmap, w, h, color, bg);
  }
  void drawXBitmap(int16_t x, int16_t y, const uint8_t bitmap[], int16_t w,
                   int16_t h, uint16_t color) {
    this->gfx_->drawXBitmap(x, y, bitmap, w, h, color);
  }

  void drawIndexedBitmap(int16_t x, int16_t y, uint8_t *bitmap,
                         uint16_t *color_index, int16_t w, int16_t h) {
    this->gfx_->drawIndexedBitmap(x, y, bitmap, color_index, w, h);
  }
  void draw3bitRGBBitmap(int16_t x, int16_t y, uint8_t *bitmap, int16_t w,
                         int16_t h) {
    this->gfx_->draw3bitRGBBitmap(x, y, bitmap, w, h);
  }

  void drawGrayscaleBitmap(int16_t x, int16_t y, const uint8_t bitmap[],
                           const uint8_t mask[], int16_t w, int16_t h) {
    this->gfx_->drawGrayscaleBitmap(x, y, bitmap, mask, w, h);
  }
  void drawGrayscaleBitmap(int16_t x, int16_t y, uint8_t *bitmap, uint8_t *mask,
                           int16_t w, int16_t h) {
    this->gfx_->drawGrayscaleBitmap(x, y, bitmap, mask, w, h);
  }
  void drawGrayscaleBitmap(int16_t x, int16_t y, const uint8_t bitmap[],
                           int16_t w, int16_t h) {
    this->gfx_->drawGrayscaleBitmap(x, y, bitmap, w, h);
  }
  void drawGrayscaleBitmap(int16_t x, int16_t y, uint8_t *bitmap, int16_t w,
                           int16_t h) {
    this->gfx_->drawGrayscaleBitmap(x, y, bitmap, w, h);
  }

  void draw16bitRGBBitmap(int16_t x, int16_t y, const uint16_t bitmap[],
                          const uint8_t mask[], int16_t w, int16_t h) {
    this->gfx_->draw16bitRGBBitmap(x, y, bitmap, mask, w, h);
  }
  void draw16bitRGBBitmap(int16_t x, int16_t y, uint16_t *bitmap, uint8_t *mask,
                          int16_t w, int16_t h) {
    this->gfx_->draw16bitRGBBitmap(x, y, bitmap, mask, w, h);
  }
  void draw16bitRGBBitmap(int16_t x, int16_t y, const uint16_t bitmap[],
                          int16_t w, int16_t h) {
    this->gfx_->draw16bitRGBBitmap(x, y, bitmap, w, h);
  }
  void draw16bitRGBBitmap(int16_t x, int16_t y, uint16_t *bitmap, int16_t w,
                          int16_t h) {
    this->gfx_->draw16bitRGBBitmap(x, y, bitmap, w, h);
  }
  void draw16bitBeRGBBitmap(int16_t x, int16_t y, uint16_t *bitmap, int16_t w,
                            int16_t h) {
    this->gfx_->draw16bitBeRGBBitmap(x, y, bitmap, w, h);
  }

  void draw24bitRGBBitmap(int16_t x, int16_t y, const uint8_t bitmap[],
                          const uint8_t mask[], int16_t w, int16_t h) {
    this->gfx_->draw24bitRGBBitmap(x, y, bitmap, mask, w, h);
  }
  void draw24bitRGBBitmap(int16_t x, int16_t y, uint8_t *bitmap, uint8_t *mask,
                          int16_t w, int16_t h) {
    this->gfx_->draw24bitRGBBitmap(x, y, bitmap, mask, w, h);
  }
  void draw24bitRGBBitmap(int16_t x, int16_t y, const uint8_t bitmap[],
                          int16_t w, int16_t h) {
    this->gfx_->draw24bitRGBBitmap(x, y, bitmap, w, h);
  }
  void draw24bitRGBBitmap(int16_t x, int16_t y, uint8_t *bitmap, int16_t w,
                          int16_t h) {
    this->gfx_->draw24bitRGBBitmap(x, y, bitmap, w, h);
  }

  void getTextBounds(const char *string, int16_t x, int16_t y, int16_t *x1,
                     int16_t *y1, uint16_t *w, uint16_t *h) {
    this->gfx_->getTextBounds(string, x, y, x1, y1, w, h);
  }
  void getTextBounds(const __FlashStringHelper *s, int16_t x, int16_t y,
                     int16_t *x1, int16_t *y1, uint16_t *w, uint16_t *h) {
    this->gfx_->getTextBounds(s, x, y, x1, y1, w, h);
  }
  void getTextBounds(const String &str, int16_t x, int16_t y, int16_t *x1,
                     int16_t *y1, uint16_t *w, uint16_t *h) {
    this->gfx_->getTextBounds(str, x, y, x1, y1, w, h);
  }

  void setTextSize(uint8_t s) { this->gfx_->setTextSize(s); }
  void setTextSize(uint8_t sx, uint8_t sy) { this->gfx_->setTextSize(sx, sy); }
  void setTextSize(uint8_t sx, uint8_t sy, uint8_t pixel_margin) {
    this->gfx_->setTextSize(sx, sy, pixel_margin);
  }

  void setFont(const GFXfont *f = NULL) { this->gfx_->setFont(f); }

  void writeSlashLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1,
                      uint16_t color) {
    this->gfx_->writeSlashLine(x0, y0, x1, y1, color);
  }

  void drawChar(int16_t x, int16_t y, unsigned char c, uint16_t color,
                uint16_t bg) {
    this->gfx_->drawChar(x, y, c, color, bg);
  }

  void setCursor(int16_t x, int16_t y) { this->gfx_->setCursor(x, y); }
  int16_t getCursorX(void) const { return this->gfx_->getCursorX(); }
  int16_t getCursorY(void) const { return this->gfx_->getCursorY(); };

  void setTextColor(uint16_t c) { this->gfx_->setTextColor(c); }

  void setTextColor(uint16_t c, uint16_t bg) {
    this->gfx_->setTextColor(c, bg);
  }

  void setTextWrap(bool w) { this->gfx_->setTextWrap(w); }

  size_t write(uint8_t chr) { return this->gfx_->write(chr); }

  int16_t width(void) const { return this->gfx_->width(); }
  int16_t height(void) const { return this->gfx_->height(); }

  uint8_t getRotation(void) const { return this->getRotation(); }

  uint16_t color565(Color clr) {
    return ((clr.red & 0xF8) << 8) | ((clr.green & 0xFC) << 3) |
           (clr.blue >> 3);
  }

  uint16_t color565(uint8_t red, uint8_t green, uint8_t blue) {
    return ((red & 0xF8) << 8) | ((green & 0xFC) << 3) | (blue >> 3);
  }

protected:
  void draw_absolute_pixel_internal(int x, int y, Color color) override {
    if (!this->get_clipping().inside(x, y))
      return;  // NOLINT
    this->gfx_->writePixelPreclipped(x, y, color565(color));
  }
  int get_width_internal() override { return this->gfx_->width(); }
  int get_height_internal() override { return this->gfx_->height(); }
  Arduino_GFX *gfx_{nullptr};
};

} // namespace graphics
} // namespace esphome
