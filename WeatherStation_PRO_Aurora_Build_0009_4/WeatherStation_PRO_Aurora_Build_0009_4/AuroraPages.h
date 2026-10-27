#pragma once

#include <Arduino.h>

#include "AuroraDisplay.h"
#include "AuroraUI.h"
#include "AuroraSettings.h"
#include "AuroraLanguage.h"
#include "AuroraNetwork.h"
#include "AuroraClock.h"
#include "AuroraLocation.h"
#include "AuroraWeather.h"
#include "AuroraPower.h"
#include "AuroraEventLog.h"

enum AuroraPageId : uint8_t {
  PAGE_HOME = 0,
  PAGE_GRAPHS = 1,
  PAGE_FORECAST = 2,
  PAGE_SETTINGS = 3,
  PAGE_ABOUT = 4,
  PAGE_COUNT = 5
};

enum AuroraOverlayPage : uint8_t {
  OVERLAY_NONE = 0,
  OVERLAY_HOURLY,
  OVERLAY_DISPLAY,
  OVERLAY_POWER,
  OVERLAY_RESET,
  OVERLAY_LANGUAGE,
  OVERLAY_NETWORK_LOCATION,
  OVERLAY_DIAGNOSTICS
};

class AuroraPages {
 public:
  AuroraPages(
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
  );

  void beginSafeState();
  void begin();
  void update();

  void showBoot(const char* message);
  void showSplashStep(
    uint8_t progress,
    AuroraTextId message
  );
  void showPortalInstructions();
  void returnToSettingsAfterPortal();
  void notifyWeatherUpdated();

 private:
  AuroraDisplay& _display;
  AuroraUI& _ui;
  AuroraSettings& _settings;
  AuroraLanguage& _language;
  AuroraNetwork& _network;
  AuroraClock& _clock;
  AuroraLocation& _location;
  AuroraWeather& _weather;
  AuroraPower& _power;
  AuroraEventLog& _eventLog;

  AuroraPageId _currentPage;
  AuroraOverlayPage _overlay;

  uint8_t _selectedForecastDay;
  uint8_t _diagnosticsPage;
  uint8_t _selectedDisplayProfile;
  uint8_t _previewDisplayProfile;

  bool _displayPreviewActive;
  unsigned long _displayPreviewStarted;

  unsigned long _lastTouchAt;
  bool _buttonWasPressed;
  unsigned long _lastButtonAt;
  unsigned long _lastClockRedraw;
  unsigned long _lastStatusRedraw;

  void drawCurrentPage();
  void drawHomePage();
  void drawGraphsPage();
  void drawForecastPage();
  void drawHourlyDetailPage();
  void drawSettingsPage();
  void drawDisplaySettingsPage();
  void drawPowerSettingsPage();
  void drawDiagnosticsPage();
  void drawDiagnosticsSystemPage();
  void drawDiagnosticsNetworkPage();
  void drawDiagnosticsWeatherPage();
  void drawDiagnosticsDisplayPage();
  void drawDiagnosticsHealthPage();
  void drawDiagnosticsEventLogPage();
  void drawDiagnosticsShell(
    const char* title,
    uint8_t pageNumber,
    uint16_t statusColor
  );
  void drawDiagnosticsRow(
    int16_t y,
    const char* label,
    const char* value,
    uint16_t valueColor,
    uint8_t valueFont = 2
  );
  void drawDiagnosticsFooter(uint8_t pageNumber);
  void drawResetPage();
  void drawLanguagePage();
  void drawNetworkLocationPage();
  void drawPortalPage();
  void drawAboutPage();

  void updateHeader();
  void updateDisplayPreviewCountdown();
  void updatePortalStatus();

  void processTouch();
  void processHardwareButton();
  void processForecastTouch(uint16_t x, uint16_t y);
  void processSettingsTouch(uint16_t x, uint16_t y);
  void processDisplayTouch(uint16_t x, uint16_t y);
  void processPowerTouch(uint16_t x, uint16_t y);
  void processResetTouch(uint16_t x, uint16_t y);
  void processLanguageTouch(uint16_t x, uint16_t y);
  void processNetworkLocationTouch(uint16_t x, uint16_t y);
  void processPortalTouch(uint16_t x, uint16_t y);
  void processDiagnosticsTouch(uint16_t x, uint16_t y);

  void nextPage();
  void closeOverlay();

  void getDayLabel(
    const char* isoDate,
    char* output,
    size_t outputSize
  ) const;

  const char* displayProfileName(uint8_t profile) const;
  const char* displayCompatibilityLabel(uint8_t profile) const;
  uint16_t displayCompatibilityColor(uint8_t profile) const;
  float displayProfileScale(uint8_t profile) const;
  void displayProfileResolution(
    uint8_t profile,
    uint16_t& width,
    uint16_t& height
  ) const;

  const char* standbyLabel(uint32_t seconds) const;

  bool isInside(
    uint16_t x,
    uint16_t y,
    int16_t left,
    int16_t top,
    int16_t width,
    int16_t height
  );
};
