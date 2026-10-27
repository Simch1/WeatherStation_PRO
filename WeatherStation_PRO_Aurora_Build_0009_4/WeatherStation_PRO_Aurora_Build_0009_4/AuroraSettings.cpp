#include "AuroraSettings.h"

void AuroraSettings::begin() {}

uint8_t AuroraSettings::loadTheme() {
  _preferences.begin("aurora-ui", true);
  const uint8_t value = _preferences.getUChar("theme", 0);
  _preferences.end();
  return value;
}

void AuroraSettings::saveTheme(uint8_t themeId) {
  _preferences.begin("aurora-ui", false);
  _preferences.putUChar("theme", themeId);
  _preferences.end();
}


uint8_t AuroraSettings::loadLanguage() {
  _preferences.begin("aurora-ui", true);
  const uint8_t value =
    _preferences.getUChar("language", 0);
  _preferences.end();

  return value <= 1 ? value : 0;
}

void AuroraSettings::saveLanguage(
  uint8_t languageId
) {
  _preferences.begin("aurora-ui", false);
  _preferences.putUChar(
    "language",
    languageId <= 1 ? languageId : 0
  );
  _preferences.end();
}

String AuroraSettings::loadSSID() {
  _preferences.begin("weather-wifi", true);
  const String value = _preferences.getString("ssid", "");
  _preferences.end();
  return value;
}

String AuroraSettings::loadPassword() {
  _preferences.begin("weather-wifi", true);
  const String value = _preferences.getString("password", "");
  _preferences.end();
  return value;
}

void AuroraSettings::saveWiFi(
  const String& ssid,
  const String& password
) {
  _preferences.begin("weather-wifi", false);
  _preferences.putString("ssid", ssid);
  _preferences.putString("password", password);
  _preferences.end();
}

AuroraLocationSettings AuroraSettings::loadLocation() {
  AuroraLocationSettings result;

  _preferences.begin("aurora-location", true);
  result.name = _preferences.getString("name", DEFAULT_LOCATION_NAME);
  result.region = _preferences.getString("region", DEFAULT_LOCATION_REGION);
  result.country = _preferences.getString("country", DEFAULT_LOCATION_COUNTRY);
  result.latitude = _preferences.getFloat("lat", DEFAULT_LOCATION_LATITUDE);
  result.longitude = _preferences.getFloat("lon", DEFAULT_LOCATION_LONGITUDE);
  _preferences.end();

  return result;
}

void AuroraSettings::saveLocation(
  const AuroraLocationSettings& location
) {
  _preferences.begin("aurora-location", false);
  _preferences.putString("name", location.name);
  _preferences.putString("region", location.region);
  _preferences.putString("country", location.country);
  _preferences.putFloat("lat", location.latitude);
  _preferences.putFloat("lon", location.longitude);
  _preferences.end();
}

String AuroraSettings::loadPendingCity() {
  _preferences.begin("aurora-location", true);
  const String value = _preferences.getString("pending", "");
  _preferences.end();
  return value;
}

void AuroraSettings::savePendingCity(const String& city) {
  _preferences.begin("aurora-location", false);
  _preferences.putString("pending", city);
  _preferences.end();
}

void AuroraSettings::clearPendingCity() {
  _preferences.begin("aurora-location", false);
  _preferences.remove("pending");
  _preferences.end();
}

uint8_t AuroraSettings::loadDisplayProfile() {
  _preferences.begin("aurora-display", true);
  const uint8_t value = _preferences.getUChar("profile", 0);
  _preferences.end();
  return value <= 3 ? value : 0;
}

void AuroraSettings::saveDisplayProfile(uint8_t profileId) {
  _preferences.begin("aurora-display", false);
  _preferences.putUChar("profile", profileId <= 3 ? profileId : 0);
  _preferences.end();
}

uint8_t AuroraSettings::loadBrightness() {
  _preferences.begin("aurora-power", true);
  const uint8_t value = _preferences.getUChar("brightness", 80);
  _preferences.end();
  return constrain(value, 20, 100);
}

void AuroraSettings::saveBrightness(uint8_t percent) {
  _preferences.begin("aurora-power", false);
  _preferences.putUChar("brightness", constrain(percent, 20, 100));
  _preferences.end();
}

uint32_t AuroraSettings::loadStandbySeconds() {
  _preferences.begin("aurora-power", true);
  const uint32_t value = _preferences.getUInt("standby", 300);
  _preferences.end();
  return value;
}

void AuroraSettings::saveStandbySeconds(uint32_t seconds) {
  _preferences.begin("aurora-power", false);
  _preferences.putUInt("standby", seconds);
  _preferences.end();
}

void AuroraSettings::factoryReset() {
  _preferences.begin("aurora-ui", false);
  _preferences.clear();
  _preferences.end();

  _preferences.begin("weather-wifi", false);
  _preferences.clear();
  _preferences.end();

  _preferences.begin("aurora-location", false);
  _preferences.clear();
  _preferences.end();

  _preferences.begin("aurora-display", false);
  _preferences.clear();
  _preferences.end();

  _preferences.begin("aurora-power", false);
  _preferences.clear();
  _preferences.end();
}
