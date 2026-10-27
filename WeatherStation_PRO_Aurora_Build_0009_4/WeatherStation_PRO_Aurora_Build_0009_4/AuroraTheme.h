#pragma once

#include <Arduino.h>
#include <TFT_eSPI.h>

enum AuroraThemeId : uint8_t {
  THEME_DARK = 0,
  THEME_LIGHT = 1
};

struct AuroraTheme {
  uint16_t background;
  uint16_t header;
  uint16_t panel;
  uint16_t card;
  uint16_t border;
  uint16_t primary;
  uint16_t text;
  uint16_t muted;
  uint16_t success;
  uint16_t warning;
  uint16_t danger;
  uint16_t button;
  uint16_t rain;
  uint16_t sun;
};

inline AuroraTheme getAuroraTheme(AuroraThemeId id) {
  if (id == THEME_LIGHT) {
    return {
      TFT_WHITE,
      0xD6DB,
      0xEF7D,
      TFT_WHITE,
      0x8410,
      0x04FF,
      TFT_BLACK,
      0x5AEB,
      TFT_GREEN,
      0xFD20,
      TFT_RED,
      0x0410,
      TFT_BLUE,
      TFT_ORANGE
    };
  }

  // Aurora UI 2.0 — Premium Nimbus.
  return {
    0x0822,  // background
    0x0843,  // header
    0x0864,  // panel
    0x10A5,  // card
    0x52AA,  // border
    0x2DFF,  // primary
    TFT_WHITE,
    0xB5B6,  // muted
    0x3FE8,  // success
    0xFD20,  // warning
    0xF986,  // danger
    0x08A6,  // button
    0x3DFF,  // rain
    0xFFE0   // sun
  };
}
