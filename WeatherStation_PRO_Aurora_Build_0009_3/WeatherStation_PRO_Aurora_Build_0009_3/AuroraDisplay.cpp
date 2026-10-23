#include "AuroraDisplay.h"

AuroraDisplay::AuroraDisplay() : _tft() {}

void AuroraDisplay::begin() {
  _tft.init();
  _tft.setRotation(DISPLAY_ROTATION);
  _tft.setTextWrap(false);
  _tft.fillScreen(TFT_BLACK);

  Serial.print("Display: ");
  Serial.print(_tft.width());
  Serial.print(" x ");
  Serial.println(_tft.height());
}

int16_t AuroraDisplay::width() const {
  return UI_WIDTH;
}

int16_t AuroraDisplay::height() const {
  return UI_HEIGHT;
}

int16_t AuroraDisplay::physicalWidth() {
  return _tft.width();
}

int16_t AuroraDisplay::physicalHeight() {
  return _tft.height();
}

void AuroraDisplay::clear(uint16_t color) {
  _tft.fillScreen(color);
}

void AuroraDisplay::setTextDatum(uint8_t datum) {
  _tft.setTextDatum(datum);
}

void AuroraDisplay::setTextColor(
  uint16_t foreground,
  uint16_t background
) {
  _tft.setTextColor(foreground, background);
}

void AuroraDisplay::drawText(
  const char* text,
  int16_t x,
  int16_t y,
  uint8_t font
) {
  _tft.drawString(text, x, y, font);
}

void AuroraDisplay::fillRect(
  int16_t x, int16_t y,
  int16_t w, int16_t h,
  uint16_t color
) {
  _tft.fillRect(x, y, w, h, color);
}

void AuroraDisplay::drawRect(
  int16_t x, int16_t y,
  int16_t w, int16_t h,
  uint16_t color
) {
  _tft.drawRect(x, y, w, h, color);
}

void AuroraDisplay::fillRoundRect(
  int16_t x, int16_t y,
  int16_t w, int16_t h,
  int16_t radius,
  uint16_t color
) {
  _tft.fillRoundRect(x, y, w, h, radius, color);
}

void AuroraDisplay::drawRoundRect(
  int16_t x, int16_t y,
  int16_t w, int16_t h,
  int16_t radius,
  uint16_t color
) {
  _tft.drawRoundRect(x, y, w, h, radius, color);
}

void AuroraDisplay::drawFastHLine(
  int16_t x, int16_t y,
  int16_t w,
  uint16_t color
) {
  _tft.drawFastHLine(x, y, w, color);
}

void AuroraDisplay::drawFastVLine(
  int16_t x, int16_t y,
  int16_t h,
  uint16_t color
) {
  _tft.drawFastVLine(x, y, h, color);
}

void AuroraDisplay::drawLine(
  int16_t x0, int16_t y0,
  int16_t x1, int16_t y1,
  uint16_t color
) {
  _tft.drawLine(x0, y0, x1, y1, color);
}

void AuroraDisplay::fillCircle(
  int16_t x,
  int16_t y,
  int16_t radius,
  uint16_t color
) {
  _tft.fillCircle(x, y, radius, color);
}

void AuroraDisplay::drawCircle(
  int16_t x,
  int16_t y,
  int16_t radius,
  uint16_t color
) {
  _tft.drawCircle(x, y, radius, color);
}

void AuroraDisplay::fillTriangle(
  int16_t x0, int16_t y0,
  int16_t x1, int16_t y1,
  int16_t x2, int16_t y2,
  uint16_t color
) {
  _tft.fillTriangle(x0, y0, x1, y1, x2, y2, color);
}

bool AuroraDisplay::readTouch(uint16_t& x, uint16_t& y) {
  return _tft.getTouch(&x, &y);
}
