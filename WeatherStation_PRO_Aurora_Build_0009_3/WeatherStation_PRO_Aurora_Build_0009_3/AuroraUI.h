#pragma once

#include <Arduino.h>
#include "AuroraDisplay.h"
#include "AuroraTheme.h"

class AuroraUI {
 public:
  explicit AuroraUI(AuroraDisplay& display);

  void setTheme(AuroraThemeId id);
  AuroraThemeId themeId() const;
  const AuroraTheme& theme() const;

  void beginPage(const char* title);
  void drawHeaderTime(const char* timeText);
  void drawWiFi(bool connected, int rssi);
  void endPage(const char* pageName, int pageNumber, int pageCount);

  void card(int16_t x, int16_t y, int16_t w, int16_t h);
  void button(
    int16_t x, int16_t y,
    int16_t w, int16_t h,
    const char* text
  );

  void navigationButton(
    int16_t x, int16_t y,
    int16_t w, int16_t h,
    const char* text
  );

  void label(
    const char* text,
    int16_t x, int16_t y,
    uint8_t font = 1
  );
  void centeredLabel(
    const char* text,
    int16_t x, int16_t y,
    uint8_t font = 2
  );
  void value(
    const char* text,
    int16_t x, int16_t y,
    uint8_t font = 4
  );

  void temperature(
    float temperature,
    int16_t centerX,
    int16_t centerY
  );
  void separator(int16_t x, int16_t y, int16_t width);

  void weatherIcon(
    int16_t x,
    int16_t y,
    int weatherCode
  );

  void smallWeatherIcon(
    int16_t x,
    int16_t y,
    int weatherCode
  );

 private:
  AuroraDisplay& _display;
  AuroraThemeId _themeId;
  AuroraTheme _theme;

  int wifiBars(int rssi) const;
  void sunIcon(int16_t x, int16_t y);
  void cloudIcon(int16_t x, int16_t y);
  void partlyCloudyIcon(int16_t x, int16_t y);
  void rainIcon(int16_t x, int16_t y);
  void fogIcon(int16_t x, int16_t y);
  void snowIcon(int16_t x, int16_t y);
  void stormIcon(int16_t x, int16_t y);
};
