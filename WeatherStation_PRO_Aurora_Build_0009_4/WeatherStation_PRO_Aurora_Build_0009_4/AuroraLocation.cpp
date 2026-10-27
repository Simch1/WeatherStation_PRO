#include "AuroraLocation.h"

#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

AuroraLocation::AuroraLocation(AuroraSettings& settings)
  : _settings(settings) {
  snprintf(_status, sizeof(_status), "Nessuna ricerca");
}

bool AuroraLocation::resolvePendingCity(bool networkAvailable) {
  String city = _settings.loadPendingCity();
  city.trim();

  if (city.length() == 0) {
    snprintf(_status, sizeof(_status), "Localita gia configurata");
    return true;
  }

  if (!networkAvailable || WiFi.status() != WL_CONNECTED) {
    snprintf(_status, sizeof(_status), "WiFi assente");
    return false;
  }

  WiFiClientSecure client;
  client.setInsecure();

  HTTPClient http;
  http.setConnectTimeout(15000);
  http.setTimeout(20000);

  String url =
    "https://geocoding-api.open-meteo.com/v1/search"
    "?count=1&language=it&format=json&name=" +
    encodeUrl(city);

  if (!http.begin(client, url)) {
    snprintf(_status, sizeof(_status), "Errore geocoding HTTPS");
    return false;
  }

  const int code = http.GET();

  if (code != HTTP_CODE_OK) {
    snprintf(_status, sizeof(_status), "Errore geocoding %d", code);
    http.end();
    return false;
  }

  const String payload = http.getString();
  http.end();

  DynamicJsonDocument document(4096);
  const DeserializationError error =
    deserializeJson(document, payload);

  if (error) {
    snprintf(_status, sizeof(_status), "Errore geocoding JSON");
    return false;
  }

  JsonArray results = document["results"].as<JsonArray>();

  if (results.isNull() || results.size() == 0) {
    snprintf(_status, sizeof(_status), "Localita non trovata");
    return false;
  }

  JsonObject first = results[0].as<JsonObject>();

  AuroraLocationSettings location;
  location.name = first["name"] | city;
  location.region = first["admin1"] | "";
  location.country = first["country"] | "";
  location.latitude = first["latitude"] | DEFAULT_LOCATION_LATITUDE;
  location.longitude = first["longitude"] | DEFAULT_LOCATION_LONGITUDE;

  _settings.saveLocation(location);
  _settings.clearPendingCity();

  snprintf(
    _status,
    sizeof(_status),
    "Salvata: %s",
    location.name.c_str()
  );

  return true;
}

const char* AuroraLocation::statusMessage() const {
  return _status;
}

String AuroraLocation::encodeUrl(const String& input) const {
  String output;

  for (size_t i = 0; i < input.length(); i++) {
    const char c = input.charAt(i);

    if (
      (c >= 'a' && c <= 'z') ||
      (c >= 'A' && c <= 'Z') ||
      (c >= '0' && c <= '9') ||
      c == '-' || c == '_' || c == '.'
    ) {
      output += c;
    } else if (c == ' ') {
      output += "%20";
    } else {
      char buffer[4];
      snprintf(buffer, sizeof(buffer), "%%%02X", static_cast<uint8_t>(c));
      output += buffer;
    }
  }

  return output;
}
