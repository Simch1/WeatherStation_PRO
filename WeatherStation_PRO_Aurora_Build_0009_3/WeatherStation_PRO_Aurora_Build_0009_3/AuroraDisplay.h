#pragma once

#include <Arduino.h>
#include <TFT_eSPI.h>
#include "AuroraConfig.h"

class AuroraDisplay {
 public:
  AuroraDisplay();

  void begin();

  int16_t width() const;
  int16_t height() const;
  int16_t physicalWidth();
  int16_t physicalHeight();

  void clear(uint16_t color);
  void setTextDatum(uint8_t datum);
  void setTextColor(uint16_t foreground, uint16_t background);
  void drawText(const char* text, int16_t x, int16_t y, uint8_t font);

  void fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
  void drawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
  void fillRoundRect(
    int16_t x, int16_t y, int16_t w, int16_t h,
    int16_t radius, uint16_t color
  );
  void drawRoundRect(
    int16_t x, int16_t y, int16_t w, int16_t h,
    int16_t radius, uint16_t color
  );
  void drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color);
  void drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color);
  void drawLine(
    int16_t x0, int16_t y0,
    int16_t x1, int16_t y1,
    uint16_t color
  );
  void fillCircle(int16_t x, int16_t y, int16_t radius, uint16_t color);
  void drawCircle(int16_t x, int16_t y, int16_t radius, uint16_t color);
  void fillTriangle(
    int16_t x0, int16_t y0,
    int16_t x1, int16_t y1,
    int16_t x2, int16_t y2,
    uint16_t color
  );

  bool readTouch(uint16_t& x, uint16_t& y);

 private:
  TFT_eSPI _tft;
};
