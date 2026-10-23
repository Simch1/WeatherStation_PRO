#pragma once

#include <Arduino.h>

#include "AuroraDisplay.h"
#include "AuroraTheme.h"

namespace AuroraUI2 {

enum InfoIcon : uint8_t {
  ICON_HUMIDITY = 0,
  ICON_WIND,
  ICON_TEMPERATURE,
  ICON_SUNRISE,
  ICON_SUNSET
};

void drawHeroCard(
  AuroraDisplay& display,
  const AuroraTheme& theme,
  int16_t x,
  int16_t y,
  int16_t w,
  int16_t h
);

void drawInfoTile(
  AuroraDisplay& display,
  const AuroraTheme& theme,
  int16_t x,
  int16_t y,
  int16_t w,
  int16_t h,
  InfoIcon icon,
  const char* label,
  const char* value,
  uint16_t accent
);

void drawUpdateBar(
  AuroraDisplay& display,
  const AuroraTheme& theme,
  const char* label,
  const char* value
);


void drawMainNavigation(
  AuroraDisplay& display,
  const AuroraTheme& theme,
  uint8_t activePage
);

void drawHomeFooter(
  AuroraDisplay& display,
  const AuroraTheme& theme,
  int pageNumber,
  int pageCount,
  const char* homeLabel,
  const char* nextLabel
);

}  // namespace AuroraUI2
