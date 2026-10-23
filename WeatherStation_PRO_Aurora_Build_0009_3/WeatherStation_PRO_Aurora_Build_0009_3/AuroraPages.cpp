#include "AuroraPages.h"
#include "AuroraUI2.h"
#include "AuroraWeatherDescriptions.h"
#include <WiFi.h>
#include "AuroraConfig.h"

AuroraPages::AuroraPages(
  AuroraDisplay& display,
  AuroraUI& ui,
  AuroraSettings& settings,
  AuroraLanguage& language,
  AuroraNetwork& network,
  AuroraClock& clockService,
  AuroraLocation& locationService,
  AuroraWeather& weatherService,
  AuroraPower& powerService,
  AuroraEventLog& eventLog
)
  : _display(display),
    _ui(ui),
    _settings(settings),
    _language(language),
    _network(network),
    _clock(clockService),
    _location(locationService),
    _weather(weatherService),
    _power(powerService),
    _eventLog(eventLog),
    _currentPage(PAGE_HOME),
    _overlay(OVERLAY_NONE),
    _selectedForecastDay(0),
    _diagnosticsPage(0),
    _selectedDisplayProfile(0),
    _previewDisplayProfile(0),
    _displayPreviewActive(false),
    _displayPreviewStarted(0),
    _lastTouchAt(0),
    _buttonWasPressed(false),
    _lastButtonAt(0),
    _lastClockRedraw(0),
    _lastStatusRedraw(0) {}

void AuroraPages::beginSafeState() {
  // Preferences e flash vengono letti soltanto dopo setup().
  // Evita accessi prematuri durante la costruzione degli oggetti globali.
  _selectedDisplayProfile =
    _settings.loadDisplayProfile();

  _previewDisplayProfile =
    _selectedDisplayProfile;

  _currentPage = PAGE_HOME;
  _overlay = OVERLAY_NONE;
  _displayPreviewActive = false;

  if (AURORA_BUTTON_PIN >= 0) {
    pinMode(
      static_cast<uint8_t>(AURORA_BUTTON_PIN),
      AURORA_BUTTON_ACTIVE_LOW ? INPUT_PULLUP : INPUT_PULLDOWN
    );
    _buttonWasPressed = false;
    _lastButtonAt = 0;
  }
}

void AuroraPages::begin() {
  drawCurrentPage();
}

void AuroraPages::update() {
  processHardwareButton();
  processTouch();
  updateDisplayPreviewCountdown();

  if (_network.isPortalActive()) {
    updatePortalStatus();
    return;
  }

  if (millis() - _lastClockRedraw >= 1000UL) {
    _lastClockRedraw = millis();
    updateHeader();
  }
}

void AuroraPages::showBoot(const char* message) {
  _display.clear(_ui.theme().background);
  _display.setTextDatum(MC_DATUM);
  _display.setTextColor(_ui.theme().primary, _ui.theme().background);
  _display.drawText("WeatherStation PRO", 160, 82, 4);
  _display.setTextColor(_ui.theme().text, _ui.theme().background);
  _display.drawText(message, 160, 133, 2);

  char versionBuffer[40];
  snprintf(
    versionBuffer,
    sizeof(versionBuffer),
    "v%s - Build %s",
    FW_VERSION,
    FW_BUILD
  );

  _display.setTextColor(_ui.theme().muted, _ui.theme().background);
  _display.drawText(versionBuffer, 160, 177, 1);
  _display.setTextDatum(TL_DATUM);
}


void AuroraPages::showSplashStep(
  uint8_t progress,
  AuroraTextId message
) {
  const uint8_t safeProgress =
    constrain(progress, 0, 100);

  _display.clear(_ui.theme().background);

  _display.setTextDatum(MC_DATUM);

  // Simbolo Aurora.
  _display.fillCircle(
    160,
    54,
    19,
    _ui.theme().primary
  );

  _display.fillCircle(
    160,
    54,
    11,
    _ui.theme().background
  );

  _display.fillTriangle(
    160,
    32,
    151,
    52,
    169,
    52,
    _ui.theme().sun
  );

  _display.setTextColor(
    _ui.theme().text,
    _ui.theme().background
  );

  _display.drawText(
    "WeatherStation PRO",
    160,
    91,
    4
  );

  _display.setTextColor(
    _ui.theme().muted,
    _ui.theme().background
  );

  _display.drawText(
    "Powered by Aurora OS",
    160,
    119,
    2
  );

  _display.fillRoundRect(
    30,
    151,
    260,
    14,
    7,
    _ui.theme().border
  );

  const int fillWidth =
    static_cast<int>(
      256.0f * safeProgress / 100.0f
    );

  if (fillWidth > 0) {
    _display.fillRoundRect(
      32,
      153,
      fillWidth,
      10,
      5,
      _ui.theme().primary
    );
  }

  char progressText[12];
  snprintf(
    progressText,
    sizeof(progressText),
    "%u%%",
    safeProgress
  );

  _display.setTextColor(
    _ui.theme().text,
    _ui.theme().background
  );

  _display.drawText(
    progressText,
    160,
    181,
    2
  );

  _display.setTextColor(
    _ui.theme().muted,
    _ui.theme().background
  );

  _display.drawText(
    _language.text(message),
    160,
    210,
    1
  );

  _display.setTextDatum(TL_DATUM);
}


void AuroraPages::showPortalInstructions() {
  drawPortalPage();
}


void AuroraPages::returnToSettingsAfterPortal() {
  _overlay = OVERLAY_NONE;
  _currentPage = PAGE_SETTINGS;
  drawSettingsPage();
}

void AuroraPages::notifyWeatherUpdated() {
  drawCurrentPage();
}

void AuroraPages::drawCurrentPage() {
  switch (_overlay) {
    case OVERLAY_HOURLY:
      drawHourlyDetailPage();
      return;
    case OVERLAY_DISPLAY:
      drawDisplaySettingsPage();
      return;
    case OVERLAY_POWER:
      drawPowerSettingsPage();
      return;
    case OVERLAY_RESET:
      drawResetPage();
      return;

    case OVERLAY_LANGUAGE:
      drawLanguagePage();
      return;

    case OVERLAY_NETWORK_LOCATION:
      drawNetworkLocationPage();
      return;

    case OVERLAY_DIAGNOSTICS:
      drawDiagnosticsPage();
      return;
    default:
      break;
  }

  switch (_currentPage) {
    case PAGE_HOME: drawHomePage(); break;
    case PAGE_GRAPHS: drawGraphsPage(); break;
    case PAGE_FORECAST: drawForecastPage(); break;
    case PAGE_SETTINGS: drawSettingsPage(); break;
    case PAGE_ABOUT: drawAboutPage(); break;
    default:
      _currentPage = PAGE_HOME;
      drawHomePage();
      break;
  }
}

void AuroraPages::drawHomePage() {
  const AuroraLocationSettings location =
    _settings.loadLocation();

  _ui.beginPage(
    location.name.c_str()
  );

  const AuroraWeatherData& weather =
    _weather.data();

  // ======================================================
  // HOME - area principale aperta, senza cornice esterna
  // ======================================================
  _display.fillRect(
    4,
    37,
    312,
    127,
    _ui.theme().panel
  );

  _display.drawFastHLine(
    12,
    160,
    296,
    _ui.theme().border
  );

  if (weather.valid) {
    char value[48];

    // ----------------------------------------------------
    // Colonna sinistra: icona grande + prossimo aggiornamento
    // ----------------------------------------------------
    const int16_t iconBaseX = 32;
    const int16_t iconBaseY = 86;

    _display.fillCircle(
      iconBaseX - 10,
      iconBaseY - 7,
      13,
      _ui.theme().text
    );

    _display.fillCircle(
      iconBaseX + 5,
      iconBaseY - 13,
      18,
      _ui.theme().text
    );

    _display.fillCircle(
      iconBaseX + 22,
      iconBaseY - 6,
      13,
      _ui.theme().text
    );

    _display.fillRoundRect(
      iconBaseX - 23,
      iconBaseY - 3,
      57,
      20,
      10,
      _ui.theme().text
    );

    _display.fillRoundRect(
      iconBaseX - 23,
      iconBaseY + 10,
      57,
      8,
      4,
      _ui.theme().primary
    );

    unsigned long remaining = 0;

    if (_weather.lastUpdateMillis() > 0) {
      const unsigned long elapsed =
        millis() - _weather.lastUpdateMillis();

      if (elapsed < WEATHER_UPDATE_INTERVAL_MS) {
        remaining =
          WEATHER_UPDATE_INTERVAL_MS - elapsed;
      }
    }

    const unsigned long minutes =
      (remaining + 59999UL) / 60000UL;

    snprintf(
      value,
      sizeof(value),
      "%lu min",
      minutes
    );

    _display.drawCircle(
      20,
      147,
      6,
      _ui.theme().primary
    );

    _display.drawLine(
      20,
      141,
      20,
      145,
      _ui.theme().primary
    );

    _display.drawLine(
      20,
      147,
      25,
      147,
      _ui.theme().primary
    );

    _display.drawLine(
      15,
      152,
      12,
      155,
      _ui.theme().primary
    );

    _display.drawLine(
      25,
      142,
      28,
      139,
      _ui.theme().primary
    );

    _display.setTextDatum(ML_DATUM);
    _display.setTextColor(
      _ui.theme().text,
      _ui.theme().panel
    );

    _display.drawText(
      _language.text(TEXT_NEXT_UPDATE),
      39,
      147,
      1
    );

    _display.setTextColor(
      _ui.theme().primary,
      _ui.theme().panel
    );

    _display.drawText(
      value,
      126,
      147,
      1
    );

    // ----------------------------------------------------
    // Colonna centrale: temperatura e stato
    // ----------------------------------------------------
    snprintf(
      value,
      sizeof(value),
      "%.1f",
      weather.temperature
    );

    _display.setTextDatum(MC_DATUM);
    _display.setTextColor(
      _ui.theme().text,
      _ui.theme().panel
    );

    _display.drawText(
      value,
      123,
      71,
      4
    );

    _display.setTextDatum(ML_DATUM);
    _display.setTextColor(
      _ui.theme().primary,
      _ui.theme().panel
    );

    _display.drawText(
      "o",
      151,
      57,
      1
    );

    _display.setTextColor(
      _ui.theme().muted,
      _ui.theme().panel
    );

    _display.drawText(
      "C",
      160,
      75,
      2
    );

    const char* weatherDescription =
      auroraWeatherDescription(
        weather.weatherCode,
        _language.language()
      );

    _display.setTextDatum(MC_DATUM);
    _display.setTextColor(
      _ui.theme().primary,
      _ui.theme().panel
    );

    if (strlen(weatherDescription) > 14) {
      char firstLine[18] = {0};
      char secondLine[18] = {0};
      const char* split = strrchr(weatherDescription, ' ');

      if (split != nullptr && split != weatherDescription) {
        const size_t firstLength =
          min(
            static_cast<size_t>(split - weatherDescription),
            sizeof(firstLine) - 1
          );

        strncpy(firstLine, weatherDescription, firstLength);
        strncpy(secondLine, split + 1, sizeof(secondLine) - 1);

        _display.drawText(firstLine, 123, 94, 1);
        _display.drawText(secondLine, 123, 109, 1);
      } else {
        _display.drawText(weatherDescription, 123, 101, 1);
      }
    } else {
      _display.drawText(weatherDescription, 123, 101, 2);
    }

    snprintf(
      value,
      sizeof(value),
      "%s %.1fC",
      _language.text(TEXT_FEELS_LIKE),
      weather.apparentTemperature
    );

    _display.setTextColor(
      _ui.theme().muted,
      _ui.theme().panel
    );

    _display.drawText(
      value,
      123,
      123,
      1
    );

    // ----------------------------------------------------
    // Colonna destra: Alba e Tramonto, due metà uguali
    // ----------------------------------------------------
    const int16_t sunAreaX = 180;
    const int16_t sunAreaY = 48;
    const int16_t sunAreaW = 128;
    const int16_t sunAreaH = 101;
    const int16_t sunHalfW = sunAreaW / 2;

    _display.drawFastVLine(
      sunAreaX,
      sunAreaY,
      sunAreaH,
      _ui.theme().border
    );

    _display.drawFastVLine(
      sunAreaX + sunHalfW,
      sunAreaY,
      sunAreaH,
      _ui.theme().border
    );

    const int16_t sunriseCenterX =
      sunAreaX + sunHalfW / 2;

    const int16_t sunsetCenterX =
      sunAreaX + sunHalfW + sunHalfW / 2;

    const int16_t sunLabelY = 60;
    const int16_t sunIconY = 91;
    const int16_t sunTimeY = 138;

    // ALBA
    _display.setTextDatum(MC_DATUM);
    _display.setTextColor(
      _ui.theme().muted,
      _ui.theme().panel
    );

    _display.drawText(
      _language.text(TEXT_SUNRISE),
      sunriseCenterX,
      sunLabelY,
      1
    );

    _display.setTextColor(
      _ui.theme().sun,
      _ui.theme().panel
    );

    _display.fillCircle(
      sunriseCenterX,
      sunIconY,
      11,
      _ui.theme().sun
    );

    _display.fillRect(
      sunriseCenterX - 13,
      sunIconY + 1,
      27,
      10,
      _ui.theme().panel
    );

    _display.drawFastHLine(
      sunriseCenterX - 19,
      sunIconY + 10,
      38,
      _ui.theme().sun
    );

    _display.drawFastHLine(
      sunriseCenterX - 13,
      sunIconY + 15,
      26,
      _ui.theme().sun
    );

    _display.drawFastHLine(
      sunriseCenterX - 8,
      sunIconY + 20,
      16,
      _ui.theme().sun
    );

    _display.drawLine(
      sunriseCenterX - 18,
      sunIconY - 12,
      sunriseCenterX - 12,
      sunIconY - 5,
      _ui.theme().sun
    );

    _display.drawLine(
      sunriseCenterX + 18,
      sunIconY - 12,
      sunriseCenterX + 12,
      sunIconY - 5,
      _ui.theme().sun
    );

    _display.drawFastVLine(
      sunriseCenterX,
      sunIconY - 22,
      9,
      _ui.theme().sun
    );

    _display.setTextColor(
      _ui.theme().text,
      _ui.theme().panel
    );

    _display.drawText(
      weather.sunrise,
      sunriseCenterX,
      sunTimeY,
      2
    );

    // TRAMONTO
    _display.setTextColor(
      _ui.theme().muted,
      _ui.theme().panel
    );

    _display.drawText(
      _language.text(TEXT_SUNSET),
      sunsetCenterX,
      sunLabelY,
      1
    );

    _display.setTextColor(
      _ui.theme().warning,
      _ui.theme().panel
    );

    _display.fillCircle(
      sunsetCenterX,
      sunIconY,
      11,
      _ui.theme().warning
    );

    _display.fillRect(
      sunsetCenterX - 13,
      sunIconY + 1,
      27,
      10,
      _ui.theme().panel
    );

    _display.drawFastHLine(
      sunsetCenterX - 19,
      sunIconY + 10,
      38,
      _ui.theme().warning
    );

    _display.drawFastHLine(
      sunsetCenterX - 13,
      sunIconY + 15,
      26,
      _ui.theme().warning
    );

    _display.drawFastHLine(
      sunsetCenterX - 8,
      sunIconY + 20,
      16,
      _ui.theme().warning
    );

    _display.drawLine(
      sunsetCenterX - 18,
      sunIconY - 12,
      sunsetCenterX - 12,
      sunIconY - 5,
      _ui.theme().warning
    );

    _display.drawLine(
      sunsetCenterX + 18,
      sunIconY - 12,
      sunsetCenterX + 12,
      sunIconY - 5,
      _ui.theme().warning
    );

    _display.drawFastVLine(
      sunsetCenterX,
      sunIconY - 22,
      9,
      _ui.theme().warning
    );

    _display.setTextColor(
      _ui.theme().text,
      _ui.theme().panel
    );

    _display.drawText(
      weather.sunset,
      sunsetCenterX,
      sunTimeY,
      2
    );

    _display.setTextDatum(TL_DATUM);

    // ====================================================
    // 4 INFO TILE UGUALI
    // ====================================================
    const int16_t tileY = 167;
    const int16_t tileH = 36;

    const int16_t tileX[4] = {
      3,
      82,
      161,
      240
    };

    const int16_t tileW[4] = {
      76,
      76,
      76,
      76
    };

    const char* tileLabels[4] = {
      "MIN/MAX",
      _language.language() == LANGUAGE_ENGLISH
        ? "HUMID."
        : "UMID.",
      _language.language() == LANGUAGE_ENGLISH
        ? "WIND"
        : "VENTO",
      "PRESS."
    };

    char tileValues[4][22];

    float minimum = weather.temperature;
    float maximum = weather.temperature;

    if (_weather.forecastValid()) {
      minimum =
        _weather.forecastDay(0).minTemperature;

      maximum =
        _weather.forecastDay(0).maxTemperature;
    }

    snprintf(
      tileValues[0],
      sizeof(tileValues[0]),
      "%.0f/%.0f C",
      minimum,
      maximum
    );

    snprintf(
      tileValues[1],
      sizeof(tileValues[1]),
      "%d %%",
      weather.humidity
    );

    snprintf(
      tileValues[2],
      sizeof(tileValues[2]),
      "%.0f km/h",
      weather.windSpeed
    );

    if (weather.pressure > 0.0f) {
      snprintf(
        tileValues[3],
        sizeof(tileValues[3]),
        "%.0f hPa",
        weather.pressure
      );
    } else {
      snprintf(
        tileValues[3],
        sizeof(tileValues[3]),
        "N/D"
      );
    }

    for (uint8_t index = 0; index < 4; index++) {
      _display.fillRoundRect(
        tileX[index],
        tileY,
        tileW[index],
        tileH,
        5,
        _ui.theme().card
      );

      _display.drawRoundRect(
        tileX[index],
        tileY,
        tileW[index],
        tileH,
        5,
        _ui.theme().border
      );

      _display.drawFastVLine(
        tileX[index] + 24,
        tileY + 5,
        tileH - 10,
        _ui.theme().primary
      );

      const int16_t iconX =
        tileX[index] + 12;

      const int16_t centerY =
        tileY + tileH / 2;

      if (index == 0) {
        _display.drawRoundRect(
          iconX - 2,
          centerY - 9,
          4,
          14,
          2,
          _ui.theme().text
        );

        _display.fillCircle(
          iconX,
          centerY + 6,
          4,
          _ui.theme().primary
        );
      }

      if (index == 1) {
        _display.fillTriangle(
          iconX,
          centerY - 9,
          iconX - 6,
          centerY + 3,
          iconX + 6,
          centerY + 3,
          _ui.theme().rain
        );

        _display.fillCircle(
          iconX,
          centerY + 3,
          6,
          _ui.theme().rain
        );
      }

      if (index == 2) {
        _display.drawFastHLine(
          iconX - 7,
          centerY - 5,
          12,
          _ui.theme().primary
        );

        _display.drawFastHLine(
          iconX - 5,
          centerY,
          14,
          _ui.theme().primary
        );

        _display.drawFastHLine(
          iconX - 7,
          centerY + 5,
          11,
          _ui.theme().primary
        );
      }

      if (index == 3) {
        _display.drawCircle(
          iconX,
          centerY,
          7,
          _ui.theme().text
        );

        _display.drawLine(
          iconX,
          centerY,
          iconX + 4,
          centerY - 4,
          _ui.theme().primary
        );
      }

      _display.setTextDatum(ML_DATUM);
      _display.setTextColor(
        _ui.theme().muted,
        _ui.theme().card
      );

      _display.drawText(
        tileLabels[index],
        tileX[index] + 29,
        tileY + 11,
        1
      );

      _display.setTextColor(
        index == 0
          ? _ui.theme().text
          : _ui.theme().primary,
        _ui.theme().card
      );

      _display.drawText(
        tileValues[index],
        tileX[index] + 29,
        tileY + 25,
        1
      );
    }
  } else {
    _ui.centeredLabel(
      _language.text(TEXT_NOT_AVAILABLE),
      160,
      82,
      2
    );

    _ui.centeredLabel(
      _weather.statusMessage(),
      160,
      109,
      1
    );
  }

  AuroraUI2::drawMainNavigation(
    _display,
    _ui.theme(),
    0
  );

  updateHeader();
}

void AuroraPages::drawGraphsPage() {
  _ui.beginPage("Grafici");

  _ui.card(
    8,
    42,
    304,
    151
  );

  _display.setTextDatum(ML_DATUM);
  _display.setTextColor(
    _ui.theme().text,
    _ui.theme().panel
  );

  _display.drawText(
    "Temperatura - prossime 24 ore",
    18,
    55,
    2
  );

  const int16_t graphX = 28;
  const int16_t graphY = 77;
  const int16_t graphW = 264;
  const int16_t graphH = 82;

  _display.drawFastVLine(
    graphX,
    graphY,
    graphH,
    _ui.theme().border
  );

  _display.drawFastHLine(
    graphX,
    graphY + graphH,
    graphW,
    _ui.theme().border
  );

  if (_weather.hourlyValid()) {
    float minimum = 1000.0f;
    float maximum = -1000.0f;

    for (
      uint8_t slot = 0;
      slot < HOURLY_SLOTS_PER_DAY;
      slot++
    ) {
      const float value =
        _weather.hourlySlot(
          0,
          slot
        ).temperature;

      if (value < minimum) minimum = value;
      if (value > maximum) maximum = value;
    }

    if (maximum - minimum < 1.0f) {
      maximum = minimum + 1.0f;
    }

    int16_t previousX = 0;
    int16_t previousY = 0;

    for (
      uint8_t slot = 0;
      slot < HOURLY_SLOTS_PER_DAY;
      slot++
    ) {
      const AuroraHourlySlot& item =
        _weather.hourlySlot(
          0,
          slot
        );

      const int16_t pointX =
        graphX +
        6 +
        slot *
        (graphW - 12) /
        (HOURLY_SLOTS_PER_DAY - 1);

      const float normalized =
        (item.temperature - minimum) /
        (maximum - minimum);

      const int16_t pointY =
        graphY +
        graphH -
        7 -
        static_cast<int16_t>(
          normalized *
          (graphH - 16)
        );

      if (slot > 0) {
        _display.drawLine(
          previousX,
          previousY,
          pointX,
          pointY,
          _ui.theme().primary
        );
      }

      _display.fillCircle(
        pointX,
        pointY,
        3,
        _ui.theme().primary
      );

      if (
        slot == 0 ||
        slot == HOURLY_SLOTS_PER_DAY - 1
      ) {
        _display.setTextDatum(MC_DATUM);
        _display.setTextColor(
          _ui.theme().muted,
          _ui.theme().panel
        );

        _display.drawText(
          item.time,
          pointX,
          graphY + graphH + 10,
          1
        );
      }

      previousX = pointX;
      previousY = pointY;
    }

    char summary[48];
    snprintf(
      summary,
      sizeof(summary),
      "Min %.1f C   Max %.1f C",
      minimum,
      maximum
    );

    _display.setTextDatum(MC_DATUM);
    _display.setTextColor(
      _ui.theme().text,
      _ui.theme().panel
    );

    _display.drawText(
      summary,
      160,
      181,
      1
    );
  } else {
    _display.setTextDatum(MC_DATUM);
    _display.setTextColor(
      _ui.theme().muted,
      _ui.theme().panel
    );

    _display.drawText(
      "Dati orari non disponibili",
      160,
      119,
      2
    );
  }

  _display.setTextDatum(TL_DATUM);

  AuroraUI2::drawMainNavigation(
    _display,
    _ui.theme(),
    1
  );

  updateHeader();
}


void AuroraPages::drawForecastPage() {
  _ui.beginPage(
    _language.text(TEXT_FORECAST)
  );

  if (!_weather.forecastValid()) {
    _ui.card(8, 43, 304, 150);
    _ui.centeredLabel(
      _language.text(TEXT_FORECAST_UNAVAILABLE),
      160,
      115,
      2
    );
  } else {
    const int cardWidth = 58;
    const int cardGap = 3;

    for (uint8_t i = 0; i < FORECAST_DAYS; i++) {
      const int x = 8 + i * (cardWidth + cardGap);
      const int centerX = x + cardWidth / 2;
      const AuroraForecastDay& day = _weather.forecastDay(i);

      _ui.card(x, 43, cardWidth, 150);

      char dayLabel[8];
      getDayLabel(day.date, dayLabel, sizeof(dayLabel));
      _ui.centeredLabel(dayLabel, centerX, 59, 2);
      _ui.smallWeatherIcon(centerX, 88, day.weatherCode);

      char buffer[16];
      snprintf(buffer, sizeof(buffer), "%.0f", day.maxTemperature);
      _ui.value(buffer, centerX, 118, 2);
      snprintf(buffer, sizeof(buffer), "%.0f", day.minTemperature);
      _ui.centeredLabel(buffer, centerX, 140, 2);
      snprintf(buffer, sizeof(buffer), "%d%%", day.rainProbability);
      _ui.centeredLabel(buffer, centerX, 166, 1);
      _display.setTextDatum(MC_DATUM);
      _display.setTextColor(
        _ui.theme().muted,
        _ui.theme().panel
      );
      _display.drawText(
        _language.text(TEXT_RAIN),
        centerX,
        181,
        1
      );
      _display.setTextDatum(TL_DATUM);
    }
  }

  AuroraUI2::drawMainNavigation(
    _display,
    _ui.theme(),
    2
  );
  updateHeader();
}

void AuroraPages::drawHourlyDetailPage() {
  const AuroraForecastDay& selected =
    _weather.forecastDay(_selectedForecastDay);

  char dayLabel[8];
  getDayLabel(selected.date, dayLabel, sizeof(dayLabel));

  char title[24];
  snprintf(
    title,
    sizeof(title),
    "%s %s",
    _language.text(TEXT_DETAIL),
    dayLabel
  );
  _ui.beginPage(title);

  // Intestazioni fisse sopra i dati.
  _display.fillRoundRect(
    8,
    41,
    304,
    22,
    6,
    _ui.theme().card
  );

  _display.setTextDatum(MC_DATUM);
  _display.setTextColor(
    _ui.theme().muted,
    _ui.theme().card
  );

  _display.drawText(_language.text(TEXT_HOUR), 27, 52, 1);
  _display.drawText(_language.text(TEXT_WEATHER), 69, 52, 1);
  _display.drawText(_language.text(TEXT_TEMPERATURE), 117, 52, 1);
  _display.drawText(_language.text(TEXT_RAIN), 169, 52, 1);
  _display.drawText(_language.text(TEXT_HUMIDITY), 222, 52, 1);
  _display.drawText(_language.text(TEXT_WIND), 278, 52, 1);
  _display.setTextDatum(TL_DATUM);

  if (!_weather.hourlyValid()) {
    _ui.card(8, 69, 304, 119);
    _ui.centeredLabel(
      _language.text(TEXT_HOURLY_UNAVAILABLE),
      160,
      128,
      2
    );
  } else {
    const int firstRowY = 65;
    const int rowHeight = 16;

    for (
      uint8_t slot = 0;
      slot < HOURLY_SLOTS_PER_DAY;
      slot++
    ) {
      const int rowTop =
        firstRowY + slot * rowHeight;

      const int rowCenter =
        rowTop + rowHeight / 2;

      const AuroraHourlySlot& item =
        _weather.hourlySlot(
          _selectedForecastDay,
          slot
        );

      const uint16_t rowColor =
        slot % 2 == 0
          ? _ui.theme().panel
          : _ui.theme().background;

      _display.fillRect(
        8,
        rowTop,
        304,
        rowHeight,
        rowColor
      );

      _display.setTextDatum(MC_DATUM);
      _display.setTextColor(
        _ui.theme().text,
        rowColor
      );

      _display.drawText(
        item.time,
        27,
        rowCenter,
        1
      );

      // L'icona cancella sul colore della card.
      // Ridisegniamo prima una piccola area uniforme.
      _display.fillRect(
        56,
        rowTop,
        27,
        rowHeight,
        _ui.theme().panel
      );

      _ui.smallWeatherIcon(
        69,
        rowCenter,
        item.weatherCode
      );

      char buffer[20];

      snprintf(
        buffer,
        sizeof(buffer),
        "%.0f C",
        item.temperature
      );

      _display.setTextColor(
        _ui.theme().text,
        rowColor
      );
      _display.drawText(
        buffer,
        117,
        rowCenter,
        1
      );

      snprintf(
        buffer,
        sizeof(buffer),
        "%d%%",
        item.rainProbability
      );
      _display.drawText(
        buffer,
        169,
        rowCenter,
        1
      );

      snprintf(
        buffer,
        sizeof(buffer),
        "%d%%",
        item.humidity
      );
      _display.drawText(
        buffer,
        222,
        rowCenter,
        1
      );

      snprintf(
        buffer,
        sizeof(buffer),
        "%.0f km/h",
        item.windSpeed
      );
      _display.drawText(
        buffer,
        278,
        rowCenter,
        1
      );

      _display.setTextDatum(TL_DATUM);
    }
  }

  _display.fillRect(
    0,
    202,
    UI_WIDTH,
    38,
    _ui.theme().background
  );

  _display.drawFastHLine(
    0,
    201,
    UI_WIDTH,
    _ui.theme().border
  );

  _ui.navigationButton(
    8,
    211,
    86,
    25,
    "< PREVISIONI"
  );

  updateHeader();
}

void AuroraPages::drawSettingsPage() {
  _ui.beginPage(
    _language.text(TEXT_SETTINGS)
  );

  // Griglia definitiva 2 x 4.
  // Wi-Fi e Localita sono funzioni indipendenti.
  const AuroraTextId labels[8] = {
    TEXT_CHANGE_WIFI,
    TEXT_CHANGE_LOCATION,
    TEXT_DISPLAY,
    TEXT_BRIGHTNESS_STANDBY,
    TEXT_LANGUAGE,
    TEXT_THEME,
    TEXT_DIAGNOSTICS_MENU,
    TEXT_FACTORY_RESET
  };

  const int leftX = 10;
  const int rightX = 164;
  const int cardW = 146;
  const int cardH = 33;
  const int firstY = 41;
  const int rowGap = 39;

  for (int index = 0; index < 8; index++) {
    const int column =
      index % 2;

    const int row =
      index / 2;

    const int x =
      column == 0
        ? leftX
        : rightX;

    const int y =
      firstY + row * rowGap;

    _ui.button(
      x,
      y,
      cardW,
      cardH,
      _language.text(labels[index])
    );
  }

  AuroraUI2::drawMainNavigation(
    _display,
    _ui.theme(),
    3
  );

  updateHeader();
}

void AuroraPages::drawDisplaySettingsPage() {
  if (_displayPreviewActive) {
    const float scale =
      displayProfileScale(_previewDisplayProfile);

    const int scalePercent =
      static_cast<int>(scale * 100.0f + 0.5f);

    uint16_t logicalW = 0;
    uint16_t logicalH = 0;

    displayProfileResolution(
      _previewDisplayProfile,
      logicalW,
      logicalH
    );

    _ui.beginPage("Anteprima display");

    // Simulazione visibile di una schermata reale.
    _ui.card(10, 43, 300, 137);

    _display.fillRoundRect(
      20,
      52,
      178,
      80,
      8,
      _ui.theme().card
    );

    _display.drawRoundRect(
      20,
      52,
      178,
      80,
      8,
      _ui.theme().border
    );

    _display.fillRoundRect(
      207,
      52,
      93,
      80,
      8,
      _ui.theme().card
    );

    _display.drawRoundRect(
      207,
      52,
      93,
      80,
      8,
      _ui.theme().border
    );

    // Più il profilo è inadatto, più l'esempio appare piccolo.
    uint8_t temperatureFont = 4;
    uint8_t dataFont = 2;

    if (scale < 0.75f) {
      temperatureFont = 2;
      dataFont = 1;
    } else if (scale < 0.95f) {
      temperatureFont = 3;
      dataFont = 1;
    }

    _display.setTextDatum(MC_DATUM);
    _display.setTextColor(
      _ui.theme().text,
      _ui.theme().card
    );

    _display.drawText(
      "24.6 C",
      109,
      81,
      temperatureFont
    );

    _display.drawText(
      "Sereno",
      109,
      111,
      dataFont
    );

    _display.drawText(
      "UMIDITA",
      253,
      69,
      1
    );

    _display.drawText(
      "52%",
      253,
      89,
      dataFont
    );

    _display.drawText(
      "VENTO",
      253,
      108,
      1
    );

    _display.drawText(
      "11 km/h",
      253,
      123,
      dataFont
    );

    char profileInfo[48];
    snprintf(
      profileInfo,
      sizeof(profileInfo),
      "%s  %u x %u",
      displayProfileName(_previewDisplayProfile),
      logicalW,
      logicalH
    );

    _display.setTextColor(
      _ui.theme().text,
      _ui.theme().panel
    );

    _display.drawText(
      profileInfo,
      160,
      145,
      2
    );

    char scaleInfo[64];
    snprintf(
      scaleInfo,
      sizeof(scaleInfo),
      "Scala %d%% - %s",
      scalePercent,
      displayCompatibilityLabel(
        _previewDisplayProfile
      )
    );

    _display.setTextColor(
      displayCompatibilityColor(
        _previewDisplayProfile
      ),
      _ui.theme().panel
    );

    _display.drawText(
      scaleInfo,
      160,
      166,
      1
    );

    _display.setTextDatum(TL_DATUM);

    _display.fillRect(
      0,
      202,
      UI_WIDTH,
      38,
      _ui.theme().background
    );

    _display.drawFastHLine(
      0,
      201,
      UI_WIDTH,
      _ui.theme().border
    );

    _ui.button(
      8,
      207,
      90,
      27,
      _language.text(TEXT_CANCEL)
    );

    _ui.button(
      177,
      207,
      134,
      27,
      _language.text(TEXT_CONFIRM)
    );

    return;
  }

  _ui.beginPage("Profilo display");

  for (uint8_t i = 0; i < 4; i++) {
    const int y = 43 + i * 34;

    uint16_t logicalW = 0;
    uint16_t logicalH = 0;

    displayProfileResolution(
      i,
      logicalW,
      logicalH
    );

    const bool selected =
      _selectedDisplayProfile == i;

    const uint16_t rowColor =
      selected
        ? _ui.theme().button
        : _ui.theme().panel;

    _display.fillRoundRect(
      10,
      y,
      300,
      29,
      7,
      rowColor
    );

    _display.drawRoundRect(
      10,
      y,
      300,
      29,
      7,
      selected
        ? _ui.theme().primary
        : _ui.theme().border
    );

    char profileText[36];

    snprintf(
      profileText,
      sizeof(profileText),
      "%s  %ux%u",
      displayProfileName(i),
      logicalW,
      logicalH
    );

    _display.setTextDatum(ML_DATUM);
    _display.setTextColor(
      _ui.theme().text,
      rowColor
    );

    _display.drawText(
      profileText,
      21,
      y + 14,
      2
    );

    _display.setTextDatum(MR_DATUM);
    _display.setTextColor(
      displayCompatibilityColor(i),
      rowColor
    );

    _display.drawText(
      displayCompatibilityLabel(i),
      298,
      y + 14,
      1
    );

    _display.setTextDatum(TL_DATUM);
  }

  _ui.centeredLabel(
    "Seleziona e premi ANTEPRIMA",
    160,
    184,
    1
  );

  _display.fillRect(
    0,
    202,
    UI_WIDTH,
    38,
    _ui.theme().background
  );

  _display.drawFastHLine(
    0,
    201,
    UI_WIDTH,
    _ui.theme().border
  );

  _ui.button(
    8,
    207,
    90,
    27,
    "< INDIETRO"
  );

  _ui.button(
    177,
    207,
    134,
    27,
    "ANTEPRIMA"
  );

  updateHeader();
}

void AuroraPages::drawPowerSettingsPage() {
  _ui.beginPage("Energia e display");

  char brightness[24];
  snprintf(brightness, sizeof(brightness), "LUMINOSITA %u%%", _power.brightness());
  _ui.centeredLabel(brightness, 160, 54, 2);

  const uint8_t levels[] = {20, 40, 60, 80, 100};
  for (uint8_t i = 0; i < 5; i++) {
    char label[8];
    snprintf(label, sizeof(label), "%u", levels[i]);
    _ui.button(12 + i * 61, 70, 52, 25, label);
  }

  char standby[40];
  snprintf(
    standby,
    sizeof(standby),
    "STANDBY: %s",
    standbyLabel(_power.standbySeconds())
  );
  _ui.centeredLabel(standby, 160, 113, 2);

  const char* timeoutLabels[] = {"MAI", "30 S", "1 MIN", "5 MIN", "10 MIN"};
  for (uint8_t i = 0; i < 5; i++) {
    _ui.button(12 + i * 61, 130, 52, 25, timeoutLabels[i]);
  }

  _ui.centeredLabel(
    _power.backlightAvailable()
      ? "Retroilluminazione rilevata"
      : "Pin BL non rilevato",
    160,
    177,
    1
  );

  _display.fillRect(0, 202, UI_WIDTH, 38, _ui.theme().background);
  _display.drawFastHLine(0, 201, UI_WIDTH, _ui.theme().border);
  _ui.button(8, 207, 90, 27, "< INDIETRO");
  updateHeader();
}

void AuroraPages::drawDiagnosticsPage() {
  switch (_diagnosticsPage) {
    case 0: drawDiagnosticsSystemPage(); break;
    case 1: drawDiagnosticsNetworkPage(); break;
    case 2: drawDiagnosticsWeatherPage(); break;
    case 3: drawDiagnosticsDisplayPage(); break;
    case 4: drawDiagnosticsHealthPage(); break;
    case 5: drawDiagnosticsEventLogPage(); break;
    default:
      _diagnosticsPage = 0;
      drawDiagnosticsSystemPage();
      break;
  }
}

void AuroraPages::drawDiagnosticsShell(
  const char* title,
  uint8_t pageNumber,
  uint16_t statusColor
) {
  _ui.beginPage(
    _language.text(TEXT_DIAGNOSTICS)
  );

  _display.fillRoundRect(
    9,
    43,
    302,
    150,
    8,
    _ui.theme().panel
  );

  _display.drawRoundRect(
    9,
    43,
    302,
    150,
    8,
    _ui.theme().border
  );

  _display.drawFastHLine(
    22,
    44,
    276,
    _ui.theme().primary
  );

  _display.setTextDatum(ML_DATUM);
  _display.setTextColor(
    _ui.theme().primary,
    _ui.theme().panel
  );
  _display.drawText(
    title,
    23,
    59,
    2
  );

  _display.fillRoundRect(
    248,
    51,
    48,
    17,
    7,
    _ui.theme().card
  );
  _display.drawRoundRect(
    248,
    51,
    48,
    17,
    7,
    statusColor
  );
  _display.fillCircle(
    260,
    59,
    4,
    statusColor
  );

  char pageLabel[8];
  snprintf(
    pageLabel,
    sizeof(pageLabel),
    "%u/6",
    pageNumber
  );

  _display.setTextDatum(MR_DATUM);
  _display.setTextColor(
    _ui.theme().muted,
    _ui.theme().card
  );
  _display.drawText(
    pageLabel,
    290,
    59,
    1
  );

  _display.setTextDatum(TL_DATUM);
}

void AuroraPages::drawDiagnosticsRow(
  int16_t y,
  const char* label,
  const char* value,
  uint16_t valueColor,
  uint8_t valueFont
) {
  _display.drawFastHLine(
    22,
    y - 15,
    276,
    _ui.theme().border
  );

  _display.setTextDatum(ML_DATUM);
  _display.setTextColor(
    _ui.theme().muted,
    _ui.theme().panel
  );
  _display.drawText(
    label,
    24,
    y,
    1
  );

  _display.setTextDatum(MR_DATUM);
  _display.setTextColor(
    valueColor,
    _ui.theme().panel
  );
  _display.drawText(
    value,
    294,
    y,
    valueFont
  );

  _display.setTextDatum(TL_DATUM);
}

void AuroraPages::drawDiagnosticsFooter(uint8_t pageNumber) {
  _display.fillRect(
    0,
    202,
    UI_WIDTH,
    38,
    _ui.theme().background
  );

  _display.drawFastHLine(
    0,
    201,
    UI_WIDTH,
    _ui.theme().border
  );

  _ui.button(8, 207, 82, 27, "< MENU");
  _ui.button(100, 207, 52, 27, "<");

  const int16_t startX = 167;
  for (uint8_t i = 0; i < 6; i++) {
    const int16_t x = startX + i * 7;
    if (i == pageNumber - 1) {
      _display.fillCircle(
        x,
        220,
        3,
        _ui.theme().primary
      );
    } else {
      _display.drawCircle(
        x,
        220,
        2,
        _ui.theme().border
      );
    }
  }

  _ui.button(220, 207, 92, 27, ">");
}


void AuroraPages::drawDiagnosticsSystemPage() {
  drawDiagnosticsShell(
    _language.text(TEXT_SYSTEM),
    1,
    _ui.theme().success
  );

  char buffer[64];

  snprintf(
    buffer,
    sizeof(buffer),
    "%s",
    FW_VERSION
  );
  drawDiagnosticsRow(
    88,
    _language.text(TEXT_FIRMWARE),
    buffer,
    _ui.theme().text
  );

  drawDiagnosticsRow(
    116,
    _language.text(TEXT_BUILD),
    FW_BUILD,
    _ui.theme().text
  );

  const unsigned long totalSeconds =
    millis() / 1000UL;

  snprintf(
    buffer,
    sizeof(buffer),
    "%02lu:%02lu:%02lu",
    totalSeconds / 3600UL,
    (totalSeconds % 3600UL) / 60UL,
    totalSeconds % 60UL
  );
  drawDiagnosticsRow(
    144,
    _language.text(TEXT_UPTIME),
    buffer,
    _ui.theme().text
  );

  snprintf(
    buffer,
    sizeof(buffer),
    "%u KB",
    ESP.getFreeHeap() / 1024
  );
  drawDiagnosticsRow(
    170,
    _language.text(TEXT_MEMORY),
    buffer,
    _ui.theme().text
  );

  drawDiagnosticsFooter(1);
  updateHeader();
}

void AuroraPages::drawDiagnosticsNetworkPage() {
  const bool connected =
    _network.isConnected();

  const int rssi =
    _network.rssi();

  uint16_t statusColor =
    _ui.theme().success;

  if (!connected) {
    statusColor = _ui.theme().danger;
  } else if (rssi < -75) {
    statusColor = _ui.theme().warning;
  }

  drawDiagnosticsShell(
    _language.text(TEXT_NETWORK_SECTION),
    2,
    statusColor
  );

  drawDiagnosticsRow(
    88,
    _language.text(TEXT_STATUS),
    connected
      ? _language.text(TEXT_CONNECTED)
      : _language.text(TEXT_DISCONNECTED),
    statusColor
  );

  drawDiagnosticsRow(
    116,
    _language.text(TEXT_NETWORK),
    _network.ssid().c_str(),
    _ui.theme().text
  );

  char buffer[32];

  snprintf(
    buffer,
    sizeof(buffer),
    "%d dBm",
    rssi
  );
  drawDiagnosticsRow(
    144,
    _language.text(TEXT_SIGNAL),
    buffer,
    rssi < -75 ? _ui.theme().warning : _ui.theme().text
  );

  const String ip =
    _network.ipAddress();

  drawDiagnosticsRow(
    170,
    _language.text(TEXT_IP_ADDRESS),
    ip.c_str(),
    connected ? _ui.theme().text : _ui.theme().muted
  );

  drawDiagnosticsFooter(2);
  updateHeader();
}

void AuroraPages::drawDiagnosticsWeatherPage() {
  const AuroraLocationSettings location =
    _settings.loadLocation();

  const bool weatherOk =
    _weather.data().valid;

  drawDiagnosticsShell(
    _language.text(TEXT_WEATHER_SECTION),
    3,
    weatherOk
      ? _ui.theme().success
      : _ui.theme().danger
  );

  drawDiagnosticsRow(
    88,
    _language.text(TEXT_PROVIDER),
    "Open-Meteo",
    _ui.theme().text
  );

  drawDiagnosticsRow(
    116,
    _language.text(TEXT_LAST_UPDATE),
    weatherOk
      ? _weather.data().updateTime
      : "--:--",
    weatherOk ? _ui.theme().success : _ui.theme().danger
  );

  drawDiagnosticsRow(
    144,
    _language.text(TEXT_LOCATION),
    location.name.c_str(),
    _ui.theme().text
  );

  char coordinates[48];

  snprintf(
    coordinates,
    sizeof(coordinates),
    "%.4f / %.4f",
    location.latitude,
    location.longitude
  );
  drawDiagnosticsRow(
    170,
    _language.text(TEXT_COORDINATES),
    coordinates,
    _ui.theme().text,
    1
  );

  drawDiagnosticsFooter(3);
  updateHeader();
}

void AuroraPages::drawDiagnosticsDisplayPage() {
  drawDiagnosticsShell(
    _language.text(TEXT_DISPLAY_SECTION),
    4,
    _ui.theme().success
  );

  char buffer[48];

  snprintf(
    buffer,
    sizeof(buffer),
    "%dx%d",
    _display.physicalWidth(),
    _display.physicalHeight()
  );

  drawDiagnosticsRow(
    88,
    _language.text(TEXT_DISPLAY),
    buffer,
    _ui.theme().text
  );

  drawDiagnosticsRow(
    116,
    _language.text(TEXT_PROFILE),
    displayProfileName(
      _settings.loadDisplayProfile()
    ),
    _ui.theme().text
  );

  snprintf(
    buffer,
    sizeof(buffer),
    "%u%%",
    _power.brightness()
  );

  drawDiagnosticsRow(
    144,
    _language.text(TEXT_BRIGHTNESS),
    buffer,
    _ui.theme().text
  );

  drawDiagnosticsRow(
    170,
    _language.text(TEXT_BACKLIGHT),
    _power.backlightAvailable()
      ? _language.text(TEXT_DETECTED)
      : _language.text(TEXT_NOT_DETECTED),
    _power.backlightAvailable()
      ? _ui.theme().success
      : _ui.theme().warning
  );

  drawDiagnosticsFooter(4);
  updateHeader();
}



void AuroraPages::drawDiagnosticsHealthPage() {
  const bool wifiOk = _network.isConnected();
  const bool weatherOk = _weather.data().valid;
  const uint32_t heapKb = ESP.getFreeHeap() / 1024U;
  const uint32_t minHeapKb = ESP.getMinFreeHeap() / 1024U;

  drawDiagnosticsShell(
    _language.text(TEXT_SYSTEM_STATUS),
    5,
    wifiOk && weatherOk
      ? _ui.theme().success
      : _ui.theme().warning
  );

  drawDiagnosticsRow(
    88,
    _language.text(TEXT_NETWORK_SECTION),
    wifiOk ? _language.text(TEXT_ONLINE) : _language.text(TEXT_OFFLINE),
    wifiOk ? _ui.theme().success : _ui.theme().danger
  );

  drawDiagnosticsRow(
    116,
    _language.text(TEXT_WEATHER_SECTION),
    weatherOk ? _language.text(TEXT_OPERATIONAL) : _language.text(TEXT_WARNING),
    weatherOk ? _ui.theme().success : _ui.theme().warning
  );

  char value[32];
  snprintf(value, sizeof(value), "%u / %u KB", heapKb, minHeapKb);
  drawDiagnosticsRow(
    144,
    _language.text(TEXT_MEMORY),
    value,
    heapKb < 80 ? _ui.theme().warning : _ui.theme().text
  );

  const unsigned long total = millis() / 1000UL;
  snprintf(
    value,
    sizeof(value),
    "%02lu:%02lu:%02lu",
    total / 3600UL,
    (total % 3600UL) / 60UL,
    total % 60UL
  );
  drawDiagnosticsRow(
    170,
    _language.text(TEXT_UPTIME),
    value,
    _ui.theme().text
  );

  drawDiagnosticsFooter(5);
  updateHeader();
}

void AuroraPages::drawDiagnosticsEventLogPage() {
  const uint8_t logCount =
    _eventLog.count();

  drawDiagnosticsShell(
    _language.text(TEXT_EVENT_LOG),
    6,
    logCount > 0
      ? _ui.theme().primary
      : _ui.theme().muted
  );

  const uint8_t visible =
    min<uint8_t>(logCount, 5);

  if (visible == 0) {
    _ui.centeredLabel(
      _language.text(TEXT_NO_EVENTS),
      160, 123, 2
    );
  } else {
    for (uint8_t row = 0; row < visible; row++) {
      AuroraEventEntry entry;
      if (!_eventLog.entry(row, entry)) continue;

      const int y = 84 + row * 21;
      char timeText[12];
      const unsigned long minutes =
        entry.timestampSeconds / 60UL;

      snprintf(
        timeText,
        sizeof(timeText),
        "%02lu:%02lu",
        minutes / 60UL,
        minutes % 60UL
      );

      uint16_t levelColor = _ui.theme().text;
      if (entry.level == EVENT_WARNING) {
        levelColor = _ui.theme().warning;
      } else if (entry.level == EVENT_ERROR) {
        levelColor = _ui.theme().danger;
      }

      _display.setTextDatum(ML_DATUM);
      _display.setTextColor(_ui.theme().muted, _ui.theme().panel);
      _display.drawText(timeText, 24, y, 1);
      _display.setTextColor(levelColor, _ui.theme().panel);
      _display.drawText(entry.message, 76, y, 1);

      if (row < visible - 1) {
        _display.drawFastHLine(
          22, y + 10, 276, _ui.theme().border
        );
      }
    }
    _display.setTextDatum(TL_DATUM);
  }

  drawDiagnosticsFooter(6);
  updateHeader();
}

void AuroraPages::drawResetPage() {
  _ui.beginPage(
    _language.text(TEXT_FACTORY_RESET)
  );
  _ui.card(12, 54, 296, 115);
  _ui.centeredLabel(_language.text(TEXT_CLEAR_SETTINGS), 160, 82, 2);
  _ui.centeredLabel(_language.text(TEXT_CLEAR_SETTINGS_DETAIL), 160, 112, 1);
  _ui.button(35, 137, 110, 25, _language.text(TEXT_CANCEL));
  _ui.button(175, 137, 110, 25, _language.text(TEXT_CONFIRM));

  _display.fillRect(0, 202, UI_WIDTH, 38, _ui.theme().background);
  updateHeader();
}



void AuroraPages::drawLanguagePage() {
  _ui.beginPage(
    _language.text(TEXT_SELECT_LANGUAGE)
  );

  _ui.card(18, 52, 284, 110);

  _ui.button(
    36,
    70,
    248,
    31,
    _language.text(TEXT_ITALIAN)
  );

  _ui.button(
    36,
    113,
    248,
    31,
    _language.text(TEXT_ENGLISH)
  );

  const char* selected =
    _language.language() == LANGUAGE_ENGLISH
      ? "EN"
      : "IT";

  char active[24];
  snprintf(
    active,
    sizeof(active),
    "ACTIVE: %s",
    selected
  );

  _ui.centeredLabel(
    active,
    160,
    179,
    1
  );

  _display.fillRect(
    0,
    202,
    UI_WIDTH,
    38,
    _ui.theme().background
  );

  _display.drawFastHLine(
    0,
    201,
    UI_WIDTH,
    _ui.theme().border
  );

  _ui.button(
    8,
    207,
    105,
    27,
    _language.text(TEXT_BACK)
  );

  updateHeader();
}

void AuroraPages::processLanguageTouch(
  uint16_t x,
  uint16_t y
) {
  if (isInside(x, y, 0, 200, 130, 40)) {
    closeOverlay();
    return;
  }

  if (isInside(x, y, 36, 70, 248, 31)) {
    _language.setLanguage(
      LANGUAGE_ITALIAN
    );

    _settings.saveLanguage(0);
    drawLanguagePage();
    return;
  }

  if (isInside(x, y, 36, 113, 248, 31)) {
    _language.setLanguage(
      LANGUAGE_ENGLISH
    );

    _settings.saveLanguage(1);
    drawLanguagePage();
  }
}



void AuroraPages::drawNetworkLocationPage() {
  _ui.beginPage(
    _language.text(TEXT_NETWORK_LOCATION)
  );

  const AuroraLocationSettings location =
    _settings.loadLocation();

  _ui.card(10, 42, 300, 145);

  _ui.label(
    _language.text(TEXT_CURRENT_NETWORK),
    22,
    55,
    1
  );

  _ui.centeredLabel(
    _network.ssid().c_str(),
    191,
    55,
    2
  );

  _ui.label(
    _language.text(TEXT_STATUS),
    22,
    78,
    1
  );

  _ui.centeredLabel(
    _network.isConnected()
      ? _language.text(TEXT_CONNECTED)
      : _language.text(TEXT_DISCONNECTED),
    191,
    78,
    2
  );

  _ui.label(
    _language.text(TEXT_IP_ADDRESS),
    22,
    101,
    1
  );

  const String ip = _network.ipAddress();

  _ui.centeredLabel(
    ip.c_str(),
    191,
    101,
    2
  );

  _ui.label(
    _language.text(TEXT_SIGNAL),
    22,
    124,
    1
  );

  char signal[20];

  snprintf(
    signal,
    sizeof(signal),
    "%d dBm",
    _network.rssi()
  );

  _ui.centeredLabel(
    signal,
    191,
    124,
    2
  );

  _ui.separator(22, 137, 276);

  _ui.label(
    _language.text(TEXT_CURRENT_LOCATION),
    22,
    151,
    1
  );

  char locationText[64];

  snprintf(
    locationText,
    sizeof(locationText),
    "%s, %s",
    location.name.c_str(),
    location.country.c_str()
  );

  _ui.centeredLabel(
    locationText,
    191,
    151,
    2
  );

  char coordinates[48];

  snprintf(
    coordinates,
    sizeof(coordinates),
    "%.4f / %.4f",
    location.latitude,
    location.longitude
  );

  _ui.centeredLabel(
    coordinates,
    191,
    173,
    1
  );

  _display.fillRect(
    0,
    202,
    UI_WIDTH,
    38,
    _ui.theme().background
  );

  _display.drawFastHLine(
    0,
    201,
    UI_WIDTH,
    _ui.theme().border
  );

  _ui.button(
    8,
    207,
    83,
    27,
    _language.text(TEXT_BACK)
  );

  _ui.button(
    99,
    207,
    104,
    27,
    _language.text(TEXT_CHANGE_WIFI)
  );

  _ui.button(
    211,
    207,
    100,
    27,
    _language.text(TEXT_CHANGE_LOCATION)
  );

  updateHeader();
}

void AuroraPages::processNetworkLocationTouch(
  uint16_t x,
  uint16_t y
) {
  if (isInside(x, y, 0, 200, 95, 40)) {
    closeOverlay();
    return;
  }

  if (isInside(x, y, 95, 200, 112, 40)) {
    _network.startPortal(PORTAL_WIFI_ONLY);
    drawPortalPage();
    return;
  }

  if (isInside(x, y, 207, 200, 113, 40)) {
    _network.startPortal(PORTAL_LOCATION_ONLY);
    drawPortalPage();
  }
}


void AuroraPages::drawPortalPage() {
  _ui.beginPage(
    _language.text(TEXT_CONFIGURATION)
  );

  _ui.card(10, 45, 300, 135);

  _ui.label(
    _language.text(TEXT_NETWORK),
    24,
    61,
    1
  );
  _ui.centeredLabel(
    SETUP_AP_NAME,
    190,
    61,
    2
  );

  _ui.label(
    _language.text(TEXT_PASSWORD),
    24,
    87,
    1
  );

  _ui.centeredLabel(
    _settings.loadLanguage() == 1 ? "OPEN" : "APERTA",
    190,
    87,
    2
  );

  _ui.label(
    _language.text(TEXT_ADDRESS),
    24,
    113,
    1
  );
  _ui.centeredLabel("192.168.4.1", 190, 113, 2);

  _ui.label(
    _language.text(TEXT_STATUS),
    24,
    139,
    1
  );
  _ui.centeredLabel(
    _language.text(TEXT_WAITING),
    190,
    139,
    2
  );

  _ui.label(
    _language.text(TEXT_TIMEOUT),
    24,
    165,
    1
  );
  _ui.centeredLabel("03:00", 190, 165, 2);

  _display.fillRect(0, 202, UI_WIDTH, 38, _ui.theme().background);
  _display.drawFastHLine(0, 201, UI_WIDTH, _ui.theme().border);
  _ui.button(8, 207, 105, 27, _language.text(TEXT_CANCEL));
  _ui.button(199, 207, 112, 27, _language.text(TEXT_RESTART_AP));
}

void AuroraPages::updatePortalStatus() {
  static unsigned long lastPortalRedraw = 0;

  if (millis() - lastPortalRedraw < 500UL) {
    return;
  }

  lastPortalRedraw = millis();

  const uint8_t clients = _network.portalClientCount();
  const unsigned long seconds = _network.portalSecondsRemaining();

  char status[36];
  snprintf(
    status,
    sizeof(status),
    clients == 0
      ? "Nessun dispositivo"
      : "%u dispositivo collegato",
    clients
  );

  char timeout[12];
  snprintf(timeout, sizeof(timeout), "%02lu:%02lu", seconds / 60UL, seconds % 60UL);

  _display.fillRect(102, 126, 190, 54, _ui.theme().panel);

  _display.setTextDatum(MC_DATUM);
  _display.setTextColor(
    clients > 0 ? _ui.theme().success : _ui.theme().text,
    _ui.theme().panel
  );
  _display.drawText(status, 190, 139, 2);

  _display.setTextColor(
    seconds <= 30 ? _ui.theme().danger : _ui.theme().warning,
    _ui.theme().panel
  );
  _display.drawText(timeout, 190, 165, 2);
  _display.setTextDatum(TL_DATUM);
}

void AuroraPages::processPortalTouch(uint16_t x, uint16_t y) {
  if (isInside(x, y, 0, 200, 130, 40)) {
    _network.cancelPortal();
    return;
  }

  if (isInside(x, y, 185, 200, 135, 40)) {
    _network.restartPortal();
    drawPortalPage();
  }
}

void AuroraPages::drawAboutPage() {
  _ui.beginPage(
    _language.text(TEXT_INFORMATION)
  );
  _ui.card(12, 48, 296, 140);
  _ui.centeredLabel(FW_NAME, 160, 73, 4);

  char buffer[40];
  snprintf(buffer, sizeof(buffer), "Versione %s", FW_VERSION);
  _ui.centeredLabel(buffer, 160, 112, 2);
  snprintf(buffer, sizeof(buffer), "Build %s", FW_BUILD);
  _ui.centeredLabel(buffer, 160, 140, 2);
  _ui.centeredLabel("Powered by Aurora OS", 160, 170, 1);

  _ui.endPage("INFO", 4, PAGE_COUNT);
  updateHeader();
}

void AuroraPages::updateHeader() {
  if (_network.isPortalActive()) return;

  char timeBuffer[6];
  _clock.getTime(timeBuffer, sizeof(timeBuffer));
  if (_currentPage == PAGE_HOME) {
    // HOME: giorno e data in alto a sinistra, ora sulla riga inferiore.
    // Non modificare la testata delle altre pagine.
    _display.fillRect(3, 2, 82, 31, _ui.theme().header);
    struct tm localNow;
    char dateLabel[20] = "--- --/--";
    if (getLocalTime(&localNow, 20)) {
      static const char* daysIt[] = {"Dom", "Lun", "Mar", "Mer", "Gio", "Ven", "Sab"};
      static const char* daysEn[] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
      const char* day = (_language.language() == LANGUAGE_ENGLISH)
        ? daysEn[localNow.tm_wday] : daysIt[localNow.tm_wday];
      snprintf(dateLabel, sizeof(dateLabel), "%s %02d/%02d",
               day, localNow.tm_mday, localNow.tm_mon + 1);
    }
    _display.setTextDatum(TL_DATUM);
    _display.setTextColor(_ui.theme().muted, _ui.theme().header);
    _display.drawText(dateLabel, 9, 3, 1);
    _display.setTextColor(_ui.theme().text, _ui.theme().header);
    _display.drawText(timeBuffer, 9, 15, 2);
  } else {
    _ui.drawHeaderTime(timeBuffer);
  }
  _ui.drawWiFi(_network.isConnected(), _network.rssi());
}

void AuroraPages::updateDisplayPreviewCountdown() {
  if (!_displayPreviewActive) {
    return;
  }

  static int lastRemaining = -1;

  const unsigned long elapsed =
    millis() - _displayPreviewStarted;

  int remaining =
    5 - static_cast<int>(elapsed / 1000UL);

  remaining = constrain(remaining, 0, 5);

  if (remaining != lastRemaining) {
    lastRemaining = remaining;

    _display.fillRect(
      103,
      176,
      114,
      19,
      _ui.theme().background
    );

    char countdown[32];

    snprintf(
      countdown,
      sizeof(countdown),
      "Ritorno tra %d",
      remaining
    );

    _display.setTextDatum(MC_DATUM);
    _display.setTextColor(
      remaining <= 2
        ? _ui.theme().danger
        : _ui.theme().warning,
      _ui.theme().background
    );

    _display.drawText(
      countdown,
      160,
      185,
      1
    );

    _display.setTextDatum(TL_DATUM);
  }

  if (elapsed >= 5000UL) {
    _displayPreviewActive = false;

    _selectedDisplayProfile =
      _settings.loadDisplayProfile();

    _previewDisplayProfile =
      _selectedDisplayProfile;

    lastRemaining = -1;
    drawDisplaySettingsPage();
  }
}

void AuroraPages::processHardwareButton() {
  if (AURORA_BUTTON_PIN < 0) {
    return;
  }

  const bool pressed =
    digitalRead(static_cast<uint8_t>(AURORA_BUTTON_PIN)) ==
    (AURORA_BUTTON_ACTIVE_LOW ? LOW : HIGH);

  if (pressed == _buttonWasPressed) {
    return;
  }

  const unsigned long now = millis();
  if (now - _lastButtonAt < AURORA_BUTTON_DEBOUNCE_MS) {
    return;
  }

  _lastButtonAt = now;
  _buttonWasPressed = pressed;

  if (!pressed) {
    return;
  }

  if (_power.wakeScreen()) {
    drawCurrentPage();
    return;
  }

  _power.noteActivity();

  if (_network.isPortalActive()) {
    return;
  }

  if (_overlay == OVERLAY_DIAGNOSTICS) {
    _diagnosticsPage = (_diagnosticsPage + 1) % 6;
    drawDiagnosticsPage();
    return;
  }

  if (_overlay != OVERLAY_NONE) {
    closeOverlay();
    return;
  }

  switch (_currentPage) {
    case PAGE_HOME:
      _currentPage = PAGE_GRAPHS;
      break;
    case PAGE_GRAPHS:
      _currentPage = PAGE_FORECAST;
      break;
    case PAGE_FORECAST:
      _currentPage = PAGE_SETTINGS;
      break;
    default:
      _currentPage = PAGE_HOME;
      break;
  }

  drawCurrentPage();
}

void AuroraPages::processTouch() {
  if (millis() - _lastTouchAt < TOUCH_DEBOUNCE_MS) return;

  uint16_t x = 0;
  uint16_t y = 0;

  if (!_display.readTouch(x, y)) return;

  _lastTouchAt = millis();

  if (_power.wakeAndConsumeTouch()) {
    drawCurrentPage();
    return;
  }

  _power.noteActivity();

  if (_network.isPortalActive()) {
    processPortalTouch(x, y);
    return;
  }

  if (_overlay == OVERLAY_HOURLY) {
    if (isInside(x, y, 0, 200, 130, 40)) closeOverlay();
    return;
  }

  if (_overlay == OVERLAY_DISPLAY) {
    processDisplayTouch(x, y);
    return;
  }

  if (_overlay == OVERLAY_POWER) {
    processPowerTouch(x, y);
    return;
  }

  if (_overlay == OVERLAY_RESET) {
    processResetTouch(x, y);
    return;
  }

  if (_overlay == OVERLAY_LANGUAGE) {
    processLanguageTouch(x, y);
    return;
  }

  if (_overlay == OVERLAY_NETWORK_LOCATION) {
    processNetworkLocationTouch(x, y);
    return;
  }

  if (_overlay == OVERLAY_DIAGNOSTICS) {
    processDiagnosticsTouch(x, y);
    return;
  }

  // Barra principale: HOME, GRAFICI, PREVISIONI, ALTRO.
  if (isInside(x, y, 0, 202, 320, 38)) {
    if (x <= 66) {
      _currentPage = PAGE_HOME;
    } else if (x <= 143) {
      _currentPage = PAGE_GRAPHS;
    } else if (x <= 240) {
      _currentPage = PAGE_FORECAST;
    } else {
      _currentPage = PAGE_SETTINGS;
    }

    _overlay = OVERLAY_NONE;
    drawCurrentPage();
    return;
  }

  if (_currentPage == PAGE_FORECAST) {
    processForecastTouch(x, y);
    if (_overlay == OVERLAY_HOURLY) return;
  }

  if (_currentPage == PAGE_SETTINGS) {
    processSettingsTouch(x, y);
    return;
  }

}

void AuroraPages::processForecastTouch(uint16_t x, uint16_t y) {
  const int cardWidth = 58;
  const int cardGap = 3;

  for (uint8_t i = 0; i < FORECAST_DAYS; i++) {
    const int cardX = 8 + i * (cardWidth + cardGap);

    if (isInside(x, y, cardX, 43, cardWidth, 150)) {
      _selectedForecastDay = i;
      _overlay = OVERLAY_HOURLY;
      drawHourlyDetailPage();
      return;
    }
  }
}

void AuroraPages::processSettingsTouch(
  uint16_t x,
  uint16_t y
) {
  if (y >= 202) {
    return;
  }

  const int leftX = 10;
  const int rightX = 164;
  const int cardW = 146;
  const int cardH = 33;
  const int firstY = 41;
  const int rowGap = 39;

  for (int index = 0; index < 8; index++) {
    const int column =
      index % 2;

    const int row =
      index / 2;

    const int cardX =
      column == 0
        ? leftX
        : rightX;

    const int cardY =
      firstY + row * rowGap;

    if (
      !isInside(
        x,
        y,
        cardX,
        cardY,
        cardW,
        cardH
      )
    ) {
      continue;
    }

    switch (index) {
      // Wi-Fi: richiede SSID e password.
      case 0:
        _network.startPortal(
          PORTAL_WIFI_ONLY
        );
        drawPortalPage();
        break;

      // Localita:
      // se la rete e connessa chiede soltanto la citta;
      // se e offline propone la configurazione completa.
      case 1:
        _network.startPortal(
          _network.isConnected()
            ? PORTAL_LOCATION_ONLY
            : PORTAL_WIFI_AND_LOCATION
        );
        drawPortalPage();
        break;

      case 2:
        _overlay = OVERLAY_DISPLAY;
        drawDisplaySettingsPage();
        break;

      case 3:
        _overlay = OVERLAY_POWER;
        drawPowerSettingsPage();
        break;

      case 4:
        _overlay = OVERLAY_LANGUAGE;
        drawLanguagePage();
        break;

      case 5: {
        const AuroraThemeId nextTheme =
          _ui.themeId() == THEME_DARK
            ? THEME_LIGHT
            : THEME_DARK;

        _ui.setTheme(nextTheme);

        _settings.saveTheme(
          static_cast<uint8_t>(nextTheme)
        );

        drawSettingsPage();
        break;
      }

      case 6:
        _diagnosticsPage = 0;
        _overlay = OVERLAY_DIAGNOSTICS;
        drawDiagnosticsPage();
        break;

      case 7:
        _overlay = OVERLAY_RESET;
        drawResetPage();
        break;
    }

    return;
  }
}

void AuroraPages::processDisplayTouch(
  uint16_t x,
  uint16_t y
) {
  if (_displayPreviewActive) {
    // Annulla anteprima e ritorna alla lista.
    if (isInside(x, y, 0, 200, 120, 40)) {
      _displayPreviewActive = false;
      _previewDisplayProfile =
        _settings.loadDisplayProfile();

      _selectedDisplayProfile =
        _settings.loadDisplayProfile();

      drawDisplaySettingsPage();
      return;
    }

    // Conferma il profilo testato.
    if (isInside(x, y, 160, 200, 160, 40)) {
      _settings.saveDisplayProfile(
        _previewDisplayProfile
      );

      _selectedDisplayProfile =
        _previewDisplayProfile;

      _displayPreviewActive = false;
      drawDisplaySettingsPage();
      return;
    }

    return;
  }

  if (isInside(x, y, 0, 200, 120, 40)) {
    closeOverlay();
    return;
  }

  for (uint8_t i = 0; i < 4; i++) {
    const int rowY = 43 + i * 34;

    if (
      isInside(
        x,
        y,
        10,
        rowY,
        300,
        29
      )
    ) {
      _selectedDisplayProfile = i;
      drawDisplaySettingsPage();
      return;
    }
  }

  if (isInside(x, y, 160, 200, 160, 40)) {
    _previewDisplayProfile =
      _selectedDisplayProfile;

    _displayPreviewActive = true;
    _displayPreviewStarted = millis();
    drawDisplaySettingsPage();
  }
}

void AuroraPages::processPowerTouch(uint16_t x, uint16_t y) {
  if (isInside(x, y, 0, 200, 110, 40)) {
    closeOverlay();
    return;
  }

  const uint8_t levels[] = {20, 40, 60, 80, 100};

  for (uint8_t i = 0; i < 5; i++) {
    if (isInside(x, y, 12 + i * 61, 70, 52, 25)) {
      _power.setBrightness(levels[i], true);
      drawPowerSettingsPage();
      return;
    }
  }

  const uint32_t timeouts[] = {0, 30, 60, 300, 600};

  for (uint8_t i = 0; i < 5; i++) {
    if (isInside(x, y, 12 + i * 61, 130, 52, 25)) {
      _power.setStandbySeconds(timeouts[i], true);
      drawPowerSettingsPage();
      return;
    }
  }
}


void AuroraPages::processDiagnosticsTouch(
  uint16_t x,
  uint16_t y
) {
  if (isInside(x, y, 0, 200, 96, 40)) {
    _diagnosticsPage = 0;
    _overlay = OVERLAY_NONE;
    _currentPage = PAGE_SETTINGS;
    drawSettingsPage();
    return;
  }

  if (isInside(x, y, 96, 200, 80, 40)) {
    _diagnosticsPage =
      _diagnosticsPage == 0 ? 5 : _diagnosticsPage - 1;
    drawDiagnosticsPage();
    return;
  }

  if (isInside(x, y, 210, 200, 110, 40)) {
    _diagnosticsPage = (_diagnosticsPage + 1) % 6;
    drawDiagnosticsPage();
  }
}

void AuroraPages::processResetTouch(uint16_t x, uint16_t y) {
  if (isInside(x, y, 35, 130, 110, 40)) {
    closeOverlay();
    return;
  }

  if (!isInside(x, y, 175, 130, 110, 40)) {
    return;
  }

  // Cancella tutte le impostazioni Aurora OS.
  _settings.factoryReset();

  // Impedisce la riconnessione automatica e cancella
  // SSID/password conservati dallo stack Wi-Fi ESP32.
  WiFi.setAutoReconnect(false);
  WiFi.disconnect(true, true);

  delay(800);
  ESP.restart();
}

void AuroraPages::nextPage() {
  _currentPage =
    static_cast<AuroraPageId>(
      (static_cast<uint8_t>(_currentPage) + 1) % PAGE_COUNT
    );

  drawCurrentPage();
}

void AuroraPages::closeOverlay() {
  _overlay = OVERLAY_NONE;
  drawCurrentPage();
}

void AuroraPages::getDayLabel(
  const char* isoDate,
  char* output,
  size_t outputSize
) const {
  int year = 0;
  int month = 0;
  int day = 0;

  if (
    sscanf(
      isoDate,
      "%d-%d-%d",
      &year,
      &month,
      &day
    ) != 3
  ) {
    snprintf(output, outputSize, "---");
    return;
  }

  struct tm dateInfo = {};
  dateInfo.tm_year = year - 1900;
  dateInfo.tm_mon = month - 1;
  dateInfo.tm_mday = day;
  dateInfo.tm_hour = 12;

  mktime(&dateInfo);

  const char* italianLabels[] = {
    "DOM",
    "LUN",
    "MAR",
    "MER",
    "GIO",
    "VEN",
    "SAB"
  };

  const char* englishLabels[] = {
    "SUN",
    "MON",
    "TUE",
    "WED",
    "THU",
    "FRI",
    "SAT"
  };

  const char* label =
    _language.language() == LANGUAGE_ENGLISH
      ? englishLabels[dateInfo.tm_wday]
      : italianLabels[dateInfo.tm_wday];

  snprintf(
    output,
    outputSize,
    "%s",
    label
  );
}

const char* AuroraPages::displayProfileName(uint8_t profile) const {
  const char* names[] = {"SMALL", "MEDIUM", "WIDE", "LARGE"};
  return names[profile <= 3 ? profile : 0];
}


void AuroraPages::displayProfileResolution(
  uint8_t profile,
  uint16_t& width,
  uint16_t& height
) const {
  switch (profile) {
    case 1:
      width = 480;
      height = 320;
      break;

    case 2:
      width = 480;
      height = 272;
      break;

    case 3:
      width = 800;
      height = 480;
      break;

    case 0:
    default:
      width = 320;
      height = 240;
      break;
  }
}

float AuroraPages::displayProfileScale(
  uint8_t profile
) const {
  uint16_t logicalW = 0;
  uint16_t logicalH = 0;

  displayProfileResolution(
    profile,
    logicalW,
    logicalH
  );

  const float scaleX =
    static_cast<float>(
      _display.physicalWidth()
    ) / logicalW;

  const float scaleY =
    static_cast<float>(
      _display.physicalHeight()
    ) / logicalH;

  return min(scaleX, scaleY);
}

const char* AuroraPages::displayCompatibilityLabel(
  uint8_t profile
) const {
  const float scale =
    displayProfileScale(profile);

  if (scale >= 0.95f) {
    return _language.text(TEXT_NATIVE);
  }

  if (scale >= 0.75f) {
    return _language.text(TEXT_ADAPTED);
  }

  return _language.text(TEXT_NOT_SUITABLE);
}

uint16_t AuroraPages::displayCompatibilityColor(
  uint8_t profile
) const {
  const float scale =
    displayProfileScale(profile);

  if (scale >= 0.95f) {
    return _ui.theme().success;
  }

  if (scale >= 0.75f) {
    return _ui.theme().warning;
  }

  return _ui.theme().danger;
}

const char* AuroraPages::standbyLabel(uint32_t seconds) const {
  switch (seconds) {
    case 0: return "MAI";
    case 30: return "30 SECONDI";
    case 60: return "1 MINUTO";
    case 300: return "5 MINUTI";
    case 600: return "10 MINUTI";
    default: return "PERSONALIZZATO";
  }
}

bool AuroraPages::isInside(
  uint16_t x,
  uint16_t y,
  int16_t left,
  int16_t top,
  int16_t width,
  int16_t height
) {
  return (
    x >= left &&
    x < left + width &&
    y >= top &&
    y < top + height
  );
}
