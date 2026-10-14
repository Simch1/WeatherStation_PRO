#include "AuroraUI.h"
#include "AuroraConfig.h"
#include <math.h>

AuroraUI::AuroraUI(AuroraDisplay& display)
  : _display(display),
    _themeId(THEME_DARK),
    _theme(getAuroraTheme(THEME_DARK)) {}

void AuroraUI::setTheme(AuroraThemeId id) {
  _themeId = id;
  _theme = getAuroraTheme(id);
}

AuroraThemeId AuroraUI::themeId() const {
  return _themeId;
}

const AuroraTheme& AuroraUI::theme() const {
  return _theme;
}

void AuroraUI::beginPage(const char* title) {
  _display.clear(_theme.background);
  _display.fillRect(0, 0, UI_WIDTH, 35, _theme.header);
  _display.drawFastHLine(0, 34, UI_WIDTH, _theme.primary);

  // Titolo centrato: stessa gerarchia visiva in tutte le schermate.
  _display.setTextDatum(MC_DATUM);
  _display.setTextColor(_theme.text, _theme.header);
  _display.drawText(title, 160, 17, 2);
  _display.setTextDatum(TL_DATUM);
}

void AuroraUI::drawHeaderTime(const char* timeText) {
  _display.fillRect(3, 3, 78, 28, _theme.header);

  // Ora sempre ad alto contrasto: bianca nel tema scuro,
  // antracite nel tema chiaro.
  _display.setTextDatum(ML_DATUM);
  _display.setTextColor(_theme.text, _theme.header);
  _display.drawText(timeText, 9, 17, 2);
  _display.setTextDatum(TL_DATUM);
}

void AuroraUI::drawWiFi(bool connected, int rssi) {
  _display.fillRect(256, 3, 63, 28, _theme.header);

  const int bars = connected ? wifiBars(rssi) : 0;

  for (int i = 0; i < 4; i++) {
    const int h = 5 + i * 4;
    const int x = 278 + i * 8;
    const int y = 27 - h;

    _display.fillRoundRect(
      x,
      y,
      5,
      h,
      2,
      i < bars ? _theme.success : _theme.border
    );
  }

  _display.setTextDatum(MR_DATUM);
  _display.setTextColor(
    connected ? _theme.text : _theme.danger,
    _theme.header
  );
  _display.drawText(
    connected ? "WiFi" : "OFF",
    270,
    17,
    1
  );

  _display.setTextDatum(TL_DATUM);
}

void AuroraUI::endPage(
  const char* pageName,
  int pageNumber,
  int pageCount
) {
  _display.fillRect(
    0,
    202,
    UI_WIDTH,
    38,
    _theme.background
  );

  _display.drawFastHLine(
    0,
    201,
    UI_WIDTH,
    _theme.border
  );

  _display.setTextDatum(ML_DATUM);
  _display.setTextColor(
    _theme.muted,
    _theme.background
  );

  _display.drawText(
    pageName,
    10,
    220,
    1
  );

  // Indicatore grafico delle pagine.
  const int dotGap = 11;
  const int startX =
    176 - ((pageCount - 1) * dotGap) / 2;

  for (int index = 0; index < pageCount; index++) {
    const int x = startX + index * dotGap;
    const bool active =
      index == pageNumber - 1;

    if (active) {
      _display.fillCircle(
        x,
        220,
        3,
        _theme.primary
      );
    } else {
      _display.drawCircle(
        x,
        220,
        3,
        _theme.border
      );
    }
  }

  navigationButton(
    226,
    211,
    86,
    25,
    "AVANTI >"
  );

  _display.setTextDatum(TL_DATUM);
}

void AuroraUI::navigationButton(
  int16_t x,
  int16_t y,
  int16_t w,
  int16_t h,
  const char* text
) {
  _display.fillRoundRect(
    x,
    y,
    w,
    h,
    8,
    _theme.primary
  );

  _display.drawRoundRect(
    x,
    y,
    w,
    h,
    8,
    _theme.text
  );

  _display.setTextDatum(MC_DATUM);
  _display.setTextColor(
    _theme.background,
    _theme.primary
  );

  _display.drawText(
    text,
    x + w / 2,
    y + h / 2,
    1
  );

  _display.setTextDatum(TL_DATUM);
}

void AuroraUI::card(
  int16_t x,
  int16_t y,
  int16_t w,
  int16_t h
) {
  // Superficie pulita con doppio livello visivo:
  // bordo esterno discreto e accento superiore.
  _display.fillRoundRect(
    x,
    y,
    w,
    h,
    9,
    _theme.panel
  );

  _display.drawRoundRect(
    x,
    y,
    w,
    h,
    9,
    _theme.border
  );

  if (w > 20) {
    _display.drawFastHLine(
      x + 10,
      y + 1,
      w - 20,
      _theme.card
    );
  }
}

void AuroraUI::button(
  int16_t x,
  int16_t y,
  int16_t w,
  int16_t h,
  const char* text
) {
  _display.fillRoundRect(
    x,
    y,
    w,
    h,
    7,
    _theme.button
  );

  _display.drawRoundRect(
    x,
    y,
    w,
    h,
    7,
    _theme.border
  );

  // Accento sottile superiore, coerente su tutti i pulsanti.
  if (w > 18) {
    _display.drawFastHLine(
      x + 9,
      y + 1,
      w - 18,
      _theme.primary
    );
  }

  _display.setTextDatum(MC_DATUM);
  _display.setTextColor(
    _theme.text,
    _theme.button
  );

  _display.drawText(
    text,
    x + w / 2,
    y + h / 2,
    1
  );

  _display.setTextDatum(TL_DATUM);
}

void AuroraUI::label(
  const char* text,
  int16_t x,
  int16_t y,
  uint8_t font
) {
  _display.setTextDatum(ML_DATUM);
  _display.setTextColor(_theme.muted, _theme.panel);
  _display.drawText(text, x, y, font);
  _display.setTextDatum(TL_DATUM);
}

void AuroraUI::centeredLabel(
  const char* text,
  int16_t x,
  int16_t y,
  uint8_t font
) {
  _display.setTextDatum(MC_DATUM);
  _display.setTextColor(_theme.text, _theme.panel);
  _display.drawText(text, x, y, font);
  _display.setTextDatum(TL_DATUM);
}

void AuroraUI::value(
  const char* text,
  int16_t x,
  int16_t y,
  uint8_t font
) {
  _display.setTextDatum(MC_DATUM);
  _display.setTextColor(_theme.primary, _theme.panel);
  _display.drawText(text, x, y, font);
  _display.setTextDatum(TL_DATUM);
}

void AuroraUI::temperature(
  float temperatureValue,
  int16_t centerX,
  int16_t centerY
) {
  char number[16];
  snprintf(number, sizeof(number), "%.1f", temperatureValue);

  // Il valore principale usa un colore ad alto contrasto.
  _display.setTextDatum(MR_DATUM);
  _display.setTextColor(_theme.text, _theme.panel);
  _display.drawText(number, centerX + 26, centerY, 4);

  // Gradi e unità separati evitano sovrapposizioni.
  _display.setTextDatum(ML_DATUM);
  _display.setTextColor(_theme.muted, _theme.panel);
  _display.drawText("o", centerX + 29, centerY - 8, 1);
  _display.drawText("C", centerX + 39, centerY + 4, 2);

  _display.setTextDatum(TL_DATUM);
}


void AuroraUI::separator(
  int16_t x,
  int16_t y,
  int16_t width
) {
  _display.drawFastHLine(x, y, width, _theme.border);
}

int AuroraUI::wifiBars(int rssi) const {
  if (rssi >= -55) return 4;
  if (rssi >= -67) return 3;
  if (rssi >= -75) return 2;
  return 1;
}

void AuroraUI::weatherIcon(
  int16_t x,
  int16_t y,
  int weatherCode
) {
  _display.fillRect(x - 31, y - 31, 62, 62, _theme.panel);

  if (weatherCode == 0) {
    sunIcon(x, y);
  } else if (weatherCode == 1 || weatherCode == 2) {
    partlyCloudyIcon(x, y);
  } else if (weatherCode == 3) {
    cloudIcon(x, y);
  } else if (weatherCode == 45 || weatherCode == 48) {
    fogIcon(x, y);
  } else if (
    (weatherCode >= 51 && weatherCode <= 67) ||
    (weatherCode >= 80 && weatherCode <= 82)
  ) {
    rainIcon(x, y);
  } else if (
    (weatherCode >= 71 && weatherCode <= 77) ||
    weatherCode == 85 ||
    weatherCode == 86
  ) {
    snowIcon(x, y);
  } else if (weatherCode >= 95) {
    stormIcon(x, y);
  } else {
    _display.drawCircle(x, y, 14, _theme.muted);
  }
}

void AuroraUI::smallWeatherIcon(
  int16_t x,
  int16_t y,
  int weatherCode
) {
  _display.fillRect(x - 13, y - 13, 26, 26, _theme.panel);
  const uint16_t cloud = _themeId == THEME_DARK ? TFT_SILVER : 0x9CF3;

  if (weatherCode == 0) {
    _display.fillCircle(x, y, 7, _theme.sun);
    _display.drawFastHLine(x - 12, y, 4, _theme.sun);
    _display.drawFastHLine(x + 9, y, 4, _theme.sun);
    _display.drawFastVLine(x, y - 12, 4, _theme.sun);
    _display.drawFastVLine(x, y + 9, 4, _theme.sun);
    return;
  }

  if (weatherCode == 1 || weatherCode == 2) {
    _display.fillCircle(x - 5, y - 5, 6, _theme.sun);
    _display.fillCircle(x - 3, y + 3, 5, cloud);
    _display.fillCircle(x + 4, y, 7, cloud);
    _display.fillRoundRect(x - 9, y + 2, 19, 7, 3, cloud);
    return;
  }

  if (weatherCode == 3) {
    _display.fillCircle(x - 5, y + 2, 6, cloud);
    _display.fillCircle(x + 3, y - 2, 8, cloud);
    _display.fillRoundRect(x - 11, y + 1, 22, 8, 3, cloud);
    return;
  }

  if (weatherCode == 45 || weatherCode == 48) {
    _display.fillCircle(x - 5, y - 3, 5, cloud);
    _display.fillCircle(x + 3, y - 5, 6, cloud);
    _display.fillRoundRect(x - 10, y - 3, 20, 6, 3, cloud);
    _display.drawFastHLine(x - 10, y + 6, 20, _theme.muted);
    _display.drawFastHLine(x - 8, y + 10, 16, _theme.muted);
    return;
  }

  if (
    (weatherCode >= 51 && weatherCode <= 67) ||
    (weatherCode >= 80 && weatherCode <= 82)
  ) {
    _display.fillCircle(x - 5, y - 3, 5, cloud);
    _display.fillCircle(x + 3, y - 5, 7, cloud);
    _display.fillRoundRect(x - 11, y - 3, 22, 7, 3, cloud);
    _display.drawLine(x - 6, y + 7, x - 8, y + 12, _theme.rain);
    _display.drawLine(x, y + 7, x - 2, y + 12, _theme.rain);
    _display.drawLine(x + 6, y + 7, x + 4, y + 12, _theme.rain);
    return;
  }

  if (
    (weatherCode >= 71 && weatherCode <= 77) ||
    weatherCode == 85 ||
    weatherCode == 86
  ) {
    _display.fillCircle(x - 5, y - 3, 5, cloud);
    _display.fillCircle(x + 3, y - 5, 7, cloud);
    _display.fillRoundRect(x - 11, y - 3, 22, 7, 3, cloud);
    _display.drawFastHLine(x - 8, y + 9, 5, _theme.text);
    _display.drawFastVLine(x - 6, y + 7, 5, _theme.text);
    _display.drawFastHLine(x + 3, y + 9, 5, _theme.text);
    _display.drawFastVLine(x + 5, y + 7, 5, _theme.text);
    return;
  }

  if (weatherCode >= 95) {
    _display.fillCircle(x - 5, y - 3, 5, cloud);
    _display.fillCircle(x + 3, y - 5, 7, cloud);
    _display.fillRoundRect(x - 11, y - 3, 22, 7, 3, cloud);
    _display.fillTriangle(
      x + 1, y + 4,
      x - 4, y + 11,
      x + 1, y + 10,
      _theme.warning
    );
    _display.fillTriangle(
      x, y + 9,
      x + 5, y + 8,
      x - 1, y + 14,
      _theme.warning
    );
    return;
  }

  _display.drawCircle(x, y, 7, _theme.muted);
}

void AuroraUI::sunIcon(int16_t x, int16_t y) {
  _display.fillCircle(x, y, 14, _theme.sun);

  for (int angle = 0; angle < 360; angle += 45) {
    const float radians = angle * 3.1415926f / 180.0f;
    const int x1 = x + cos(radians) * 21;
    const int y1 = y + sin(radians) * 21;
    const int x2 = x + cos(radians) * 28;
    const int y2 = y + sin(radians) * 28;
    _display.drawLine(x1, y1, x2, y2, _theme.sun);
  }
}

void AuroraUI::cloudIcon(int16_t x, int16_t y) {
  const uint16_t cloud = _themeId == THEME_DARK
    ? TFT_SILVER
    : 0x9CF3;

  // Ombra leggera.
  _display.fillCircle(x - 12, y + 6, 12, _theme.border);
  _display.fillCircle(x + 1, y - 5, 17, _theme.border);
  _display.fillCircle(x + 16, y + 6, 12, _theme.border);
  _display.fillRoundRect(x - 25, y + 3, 52, 20, 9, _theme.border);

  // Corpo principale.
  _display.fillCircle(x - 13, y + 3, 11, cloud);
  _display.fillCircle(x, y - 8, 15, cloud);
  _display.fillCircle(x + 15, y + 3, 11, cloud);
  _display.fillRoundRect(x - 25, y, 51, 19, 8, cloud);
}

void AuroraUI::partlyCloudyIcon(int16_t x, int16_t y) {
  // Sole sullo sfondo.
  _display.fillCircle(x - 13, y - 12, 10, _theme.sun);
  _display.drawFastHLine(x - 28, y - 12, 6, _theme.sun);
  _display.drawFastVLine(x - 13, y - 27, 6, _theme.sun);
  _display.drawLine(x - 24, y - 23, x - 20, y - 19, _theme.sun);

  // Nuvola in primo piano.
  cloudIcon(x + 5, y + 5);
}


void AuroraUI::rainIcon(int16_t x, int16_t y) {
  cloudIcon(x, y - 8);

  for (int offset = -16; offset <= 16; offset += 16) {
    _display.drawLine(
      x + offset,
      y + 17,
      x + offset - 4,
      y + 27,
      _theme.rain
    );
  }
}

void AuroraUI::fogIcon(int16_t x, int16_t y) {
  cloudIcon(x, y - 10);

  for (int row = 0; row < 3; row++) {
    _display.drawFastHLine(
      x - 24,
      y + 15 + row * 7,
      48,
      _theme.muted
    );
  }
}

void AuroraUI::snowIcon(int16_t x, int16_t y) {
  cloudIcon(x, y - 10);

  for (int offset = -14; offset <= 14; offset += 14) {
    _display.drawFastHLine(x + offset - 4, y + 23, 9, _theme.text);
    _display.drawFastVLine(x + offset, y + 19, 9, _theme.text);
  }
}

void AuroraUI::stormIcon(int16_t x, int16_t y) {
  cloudIcon(x, y - 9);

  _display.fillTriangle(
    x + 2, y + 9,
    x - 8, y + 27,
    x, y + 24,
    _theme.warning
  );

  _display.fillTriangle(
    x, y + 22,
    x + 8, y + 20,
    x - 3, y + 36,
    _theme.warning
  );
}
