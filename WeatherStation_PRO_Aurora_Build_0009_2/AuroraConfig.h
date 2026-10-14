#pragma once

#include <Arduino.h>

constexpr const char* FW_NAME = "WeatherStation PRO";
constexpr const char* FW_VERSION = "0.17.1-home-master-v2-test";
constexpr const char* FW_BUILD = "Home Master v2 TFT Test";

constexpr int16_t UI_WIDTH = 320;
constexpr int16_t UI_HEIGHT = 240;
constexpr uint8_t DISPLAY_ROTATION = 1;
constexpr unsigned long TOUCH_DEBOUNCE_MS = 280UL;

// Pulsante fisico opzionale per riattivare lo schermo e cambiare pagina.
// Imposta qui il GPIO usato; -1 lo lascia disabilitato.
// Collegamento consigliato: pulsante tra GPIO e GND, con pull-up interno.
constexpr int8_t AURORA_BUTTON_PIN = 32;
constexpr bool AURORA_BUTTON_ACTIVE_LOW = true;
constexpr unsigned long AURORA_BUTTON_DEBOUNCE_MS = 220UL;

constexpr const char* DEFAULT_LOCATION_NAME = "Roma";
constexpr const char* DEFAULT_LOCATION_REGION = "Lazio";
constexpr const char* DEFAULT_LOCATION_COUNTRY = "Italia";
constexpr float DEFAULT_LOCATION_LATITUDE = 41.9028f;
constexpr float DEFAULT_LOCATION_LONGITUDE = 12.4964f;

constexpr const char* SETUP_AP_NAME = "WeatherStation_Setup";
constexpr const char* SETUP_AP_PASSWORD = "meteo123";

constexpr const char* TIMEZONE_ITALY =
  "CET-1CEST,M3.5.0,M10.5.0/3";

constexpr const char* NTP_SERVER_1 = "pool.ntp.org";
constexpr const char* NTP_SERVER_2 = "time.google.com";
constexpr const char* NTP_SERVER_3 = "time.cloudflare.com";

constexpr unsigned long WEATHER_UPDATE_INTERVAL_MS =
  30UL * 60UL * 1000UL;

constexpr unsigned long WIFI_RECONNECT_INTERVAL_MS =
  10000UL;

// Se TFT_eSPI definisce TFT_BL, AuroraPower lo usa automaticamente.
// In caso contrario la regolazione resta disponibile nel menu ma
// non modifica fisicamente la retroilluminazione.
constexpr bool AURORA_BACKLIGHT_ACTIVE_HIGH = true;

constexpr unsigned long CONFIG_PORTAL_TIMEOUT_MS = 180000UL;
