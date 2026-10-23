#pragma once

#include <Arduino.h>
#include <Preferences.h>
#include "AuroraConfig.h"

struct AuroraLocationSettings {
  String name;
  String region;
  String country;
  float latitude;
  float longitude;
};

class AuroraSettings {
 public:
  void begin();

  uint8_t loadTheme();
  void saveTheme(uint8_t themeId);

  uint8_t loadLanguage();
  void saveLanguage(uint8_t languageId);

  String loadSSID();
  String loadPassword();
  void saveWiFi(const String& ssid, const String& password);

  AuroraLocationSettings loadLocation();
  void saveLocation(const AuroraLocationSettings& location);

  String loadPendingCity();
  void savePendingCity(const String& city);
  void clearPendingCity();

  uint8_t loadDisplayProfile();
  void saveDisplayProfile(uint8_t profileId);

  uint8_t loadBrightness();
  void saveBrightness(uint8_t percent);

  uint32_t loadStandbySeconds();
  void saveStandbySeconds(uint32_t seconds);

  void factoryReset();

 private:
  Preferences _preferences;
};
