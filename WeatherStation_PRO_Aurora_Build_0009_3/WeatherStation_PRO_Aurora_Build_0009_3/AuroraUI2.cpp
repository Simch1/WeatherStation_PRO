#include "AuroraUI2.h"

#include <TFT_eSPI.h>

namespace {

void drawDrop(
  AuroraDisplay& display,
  int16_t x,
  int16_t y,
  uint16_t color
) {
  display.fillCircle(x, y + 3, 5, color);
  display.fillTriangle(
    x, y - 8,
    x - 5, y + 2,
    x + 5, y + 2,
    color
  );
  display.fillCircle(x, y + 3, 2, 0x0822);
}

void drawWind(
  AuroraDisplay& display,
  int16_t x,
  int16_t y,
  uint16_t color
) {
  display.drawFastHLine(x - 8, y - 5, 13, color);
  display.drawCircle(x + 5, y - 5, 3, color);
  display.drawFastHLine(x - 10, y, 18, color);
  display.drawFastHLine(x - 6, y + 5, 12, color);
  display.drawCircle(x + 6, y + 5, 3, color);
}

void drawThermometer(
  AuroraDisplay& display,
  int16_t x,
  int16_t y,
  uint16_t color
) {
  display.drawRoundRect(
    x - 3, y - 9,
    6, 15,
    3, color
  );
  display.fillCircle(x, y + 6, 5, color);
  display.drawFastVLine(x, y - 5, 10, color);
}

void drawSunrise(
  AuroraDisplay& display,
  int16_t x,
  int16_t y,
  uint16_t color
) {
  display.fillCircle(x, y + 3, 5, color);
  display.fillRect(x - 7, y + 3, 15, 6, 0x0864);
  display.drawFastHLine(x - 10, y + 8, 20, color);
  display.drawFastVLine(x, y - 8, 5, color);
  display.drawLine(x - 8, y - 4, x - 5, y - 1, color);
  display.drawLine(x + 8, y - 4, x + 5, y - 1, color);
}

void drawSunset(
  AuroraDisplay& display,
  int16_t x,
  int16_t y,
  uint16_t color
) {
  // Sole arancione che scende sotto l'orizzonte:
  // più riconoscibile della precedente icona generica.
  display.fillCircle(x, y + 1, 6, color);
  display.fillRect(x - 8, y + 1, 17, 8, 0x0864);

  display.drawFastHLine(
    x - 11,
    y + 8,
    23,
    color
  );

  display.drawFastHLine(
    x - 8,
    y + 11,
    17,
    color
  );

  display.drawLine(
    x - 8,
    y - 5,
    x - 5,
    y - 2,
    color
  );

  display.drawLine(
    x + 8,
    y - 5,
    x + 5,
    y - 2,
    color
  );

  // Freccia verso il basso.
  display.drawFastVLine(
    x,
    y - 10,
    5,
    color
  );

  display.drawLine(
    x,
    y - 5,
    x - 3,
    y - 8,
    color
  );

  display.drawLine(
    x,
    y - 5,
    x + 3,
    y - 8,
    color
  );
}

void drawIcon(
  AuroraDisplay& display,
  AuroraUI2::InfoIcon icon,
  int16_t x,
  int16_t y,
  uint16_t color
) {
  switch (icon) {
    case AuroraUI2::ICON_HUMIDITY:
      drawDrop(display, x, y, color);
      break;
    case AuroraUI2::ICON_WIND:
      drawWind(display, x, y, color);
      break;
    case AuroraUI2::ICON_TEMPERATURE:
      drawThermometer(display, x, y, color);
      break;
    case AuroraUI2::ICON_SUNRISE:
      drawSunrise(display, x, y, color);
      break;
    case AuroraUI2::ICON_SUNSET:
      drawSunset(display, x, y, color);
      break;
  }
}

}  // namespace

namespace AuroraUI2 {

void drawHeroCard(
  AuroraDisplay& display,
  const AuroraTheme& theme,
  int16_t x,
  int16_t y,
  int16_t w,
  int16_t h
) {
  display.fillRoundRect(
    x + 1,
    y + 2,
    w,
    h,
    10,
    theme.background
  );

  display.fillRoundRect(
    x,
    y,
    w,
    h,
    10,
    theme.panel
  );

  display.drawRoundRect(
    x,
    y,
    w,
    h,
    10,
    theme.border
  );

  display.drawFastHLine(
    x + 14,
    y + 1,
    w - 28,
    theme.primary
  );
}

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
) {
  display.fillRoundRect(
    x + 1,
    y + 2,
    w,
    h,
    8,
    theme.background
  );

  display.fillRoundRect(
    x,
    y,
    w,
    h,
    8,
    theme.panel
  );

  display.drawRoundRect(
    x,
    y,
    w,
    h,
    8,
    theme.border
  );

  display.drawFastHLine(
    x + 12,
    y + 1,
    w - 24,
    accent
  );

  drawIcon(
    display,
    icon,
    x + 18,
    y + h / 2,
    accent
  );

  display.drawFastVLine(
    x + 36,
    y + 6,
    h - 12,
    theme.border
  );

  display.setTextDatum(ML_DATUM);
  display.setTextColor(
    theme.muted,
    theme.panel
  );
  display.drawText(
    label,
    x + 44,
    y + 9,
    1
  );

  display.setTextColor(
    theme.text,
    theme.panel
  );
  display.drawText(
    value,
    x + 44,
    y + 22,
    2
  );

  display.setTextDatum(TL_DATUM);
}

void drawUpdateBar(
  AuroraDisplay& display,
  const AuroraTheme& theme,
  const char* label,
  const char* value
) {
  const int16_t x = 8;
  const int16_t y = 193;
  const int16_t w = 304;
  const int16_t h = 12;

  display.fillRoundRect(
    x,
    y,
    w,
    h,
    5,
    theme.panel
  );

  display.drawRoundRect(
    x,
    y,
    w,
    h,
    5,
    theme.border
  );

  display.drawCircle(
    x + 11,
    y + 6,
    4,
    theme.primary
  );

  display.drawLine(
    x + 11,
    y + 2,
    x + 11,
    y + 6,
    theme.primary
  );

  display.drawLine(
    x + 11,
    y + 6,
    x + 14,
    y + 6,
    theme.primary
  );

  display.setTextDatum(ML_DATUM);
  display.setTextColor(
    theme.muted,
    theme.panel
  );
  display.drawText(
    label,
    x + 21,
    y + 6,
    1
  );

  display.setTextDatum(MR_DATUM);
  display.setTextColor(
    theme.primary,
    theme.panel
  );
  display.drawText(
    value,
    x + w - 8,
    y + 6,
    1
  );

  display.setTextDatum(TL_DATUM);
}

void drawHomeFooter(
  AuroraDisplay& display,
  const AuroraTheme& theme,
  int pageNumber,
  int pageCount,
  const char* homeLabel,
  const char* nextLabel
) {
  display.fillRect(
    0,
    207,
    320,
    33,
    theme.background
  );

  display.drawFastHLine(
    0,
    207,
    320,
    theme.border
  );

  const int16_t buttonY = 211;
  const int16_t buttonW = 86;
  const int16_t buttonH = 25;
  const int16_t radius = 8;

  const int16_t leftX = 8;
  const int16_t centerX = 117;
  const int16_t rightX = 226;

  const uint16_t fill = theme.primary;
  const uint16_t border = theme.text;
  const uint16_t text = theme.background;

  // INDIETRO
  display.fillRoundRect(
    leftX,
    buttonY,
    buttonW,
    buttonH,
    radius,
    fill
  );

  display.drawRoundRect(
    leftX,
    buttonY,
    buttonW,
    buttonH,
    radius,
    border
  );

  // HOME
  display.fillRoundRect(
    centerX,
    buttonY,
    buttonW,
    buttonH,
    radius,
    fill
  );

  display.drawRoundRect(
    centerX,
    buttonY,
    buttonW,
    buttonH,
    radius,
    border
  );

  // AVANTI
  display.fillRoundRect(
    rightX,
    buttonY,
    buttonW,
    buttonH,
    radius,
    fill
  );

  display.drawRoundRect(
    rightX,
    buttonY,
    buttonW,
    buttonH,
    radius,
    border
  );

  display.setTextDatum(MC_DATUM);
  display.setTextColor(
    text,
    fill
  );

  display.drawText(
    "< INDIETRO",
    leftX + buttonW / 2,
    buttonY + buttonH / 2,
    1
  );

  display.drawText(
    homeLabel,
    centerX + buttonW / 2,
    buttonY + buttonH / 2,
    1
  );

  display.drawText(
    nextLabel,
    rightX + buttonW / 2 - 4,
    buttonY + buttonH / 2,
    1
  );

  display.fillTriangle(
    rightX + 71,
    buttonY + 8,
    rightX + 71,
    buttonY + 17,
    rightX + 78,
    buttonY + 12,
    text
  );

  display.setTextDatum(TL_DATUM);
}


void drawMainNavigation(
  AuroraDisplay& display,
  const AuroraTheme& theme,
  uint8_t activePage
) {
  constexpr int16_t y = 207;
  constexpr int16_t h = 29;
  constexpr int16_t radius = 7;

  // Quattro pulsanti bilanciati e compatibili con IT/EN.
  const int16_t x[4] = {
    4,
    82,
    160,
    238
  };

  const int16_t w[4] = {
    76,
    76,
    76,
    76
  };

  const char* labels[4] = {
    "HOME",
    "GRAF",
    "METEO",
    "MORE"
  };

  display.fillRect(
    0,
    204,
    320,
    36,
    theme.background
  );

  display.drawFastHLine(
    0,
    204,
    320,
    theme.border
  );

  for (uint8_t index = 0; index < 4; index++) {
    const bool active =
      index == activePage;

    const uint16_t fill =
      0x045F;

    const uint16_t border =
      active
        ? theme.primary
        : theme.border;

    const uint16_t content =
      theme.text;

    display.fillRoundRect(
      x[index],
      y,
      w[index],
      h,
      radius,
      fill
    );

    display.drawFastHLine(
      x[index] + 9,
      y + 2,
      w[index] - 18,
      active ? theme.text : theme.primary
    );

    display.drawRoundRect(
      x[index],
      y,
      w[index],
      h,
      radius,
      border
    );

    const int16_t iconX =
      x[index] + 13;

    const int16_t iconY =
      y + h / 2;

    // HOME
    if (index == 0) {
      display.fillTriangle(
        iconX - 6, iconY - 1,
        iconX,     iconY - 7,
        iconX + 6, iconY - 1,
        content
      );

      display.fillRect(
        iconX - 4,
        iconY - 1,
        8,
        7,
        content
      );

      display.fillRect(
        iconX - 1,
        iconY + 2,
        3,
        4,
        fill
      );
    }

    // GRAFICI
    if (index == 1) {
      display.drawFastVLine(
        iconX - 6,
        iconY - 6,
        13,
        content
      );

      display.drawFastHLine(
        iconX - 6,
        iconY + 6,
        13,
        content
      );

      display.drawLine(
        iconX - 4, iconY + 3,
        iconX - 1, iconY,
        content
      );

      display.drawLine(
        iconX - 1, iconY,
        iconX + 2, iconY + 2,
        content
      );

      display.drawLine(
        iconX + 2, iconY + 2,
        iconX + 6, iconY - 5,
        content
      );
    }

    // PREVISIONI
    if (index == 2) {
      display.drawCircle(
        iconX - 3,
        iconY - 3,
        4,
        content
      );

      display.fillCircle(
        iconX + 3,
        iconY + 2,
        4,
        content
      );

      display.fillCircle(
        iconX - 3,
        iconY + 3,
        3,
        content
      );

      display.fillRoundRect(
        iconX - 6,
        iconY + 1,
        12,
        6,
        3,
        content
      );
    }

    // ALTRO
    if (index == 3) {
      for (int8_t row = -5; row <= 5; row += 5) {
        display.drawFastHLine(
          iconX - 6,
          iconY + row,
          12,
          content
        );
      }
    }

    display.setTextDatum(ML_DATUM);
    display.setTextColor(
      content,
      fill
    );

    display.drawText(
      labels[index],
      x[index] + 30,
      iconY,
      1
    );
  }

  display.setTextDatum(TL_DATUM);
}


}  // namespace AuroraUI2
