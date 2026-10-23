#include "AuroraWeather.h"
#include "AuroraConfig.h"

#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

AuroraWeather::AuroraWeather(AuroraSettings& settings)
  : _settings(settings),
    _forecastValid(false),
    _hourlyValid(false),
    _lastUpdate(0) {
  begin();
}

void AuroraWeather::begin() {
  _data.temperature = 0.0f;
  _data.apparentTemperature = 0.0f;
  _data.windSpeed = 0.0f;
  _data.pressure = 0.0f;
  _data.humidity = 0;
  _data.weatherCode = -1;
  snprintf(_data.sunrise, sizeof(_data.sunrise), "--:--");
  snprintf(_data.sunset, sizeof(_data.sunset), "--:--");
  snprintf(_data.updateTime, sizeof(_data.updateTime), "--:--");
  _data.valid = false;

  clearForecast();
  clearHourly();

  snprintf(_status, sizeof(_status), "In attesa");
}

bool AuroraWeather::update(bool networkAvailable) {
  if (
    !networkAvailable ||
    WiFi.status() != WL_CONNECTED
  ) {
    snprintf(
      _status,
      sizeof(_status),
      "WiFi assente"
    );
    return false;
  }

  snprintf(
    _status,
    sizeof(_status),
    "Download in corso"
  );

  Serial.print("Heap prima HTTPS: ");
  Serial.println(ESP.getFreeHeap());

  WiFiClientSecure client;
  client.setInsecure();

  // La versione ESP32 core 3.3.10 usa NetworkClientSecure,
  // che non espone setBufferSizes(). Manteniamo i buffer
  // predefiniti e riduciamo la memoria tramite parsing streaming.

  HTTPClient http;
  http.setConnectTimeout(15000);
  http.setTimeout(25000);

  // HTTP/1.0 evita problemi con alcuni stream chunked
  // e rende più stabile il parsing diretto.
  http.useHTTP10(true);

  const String url = buildUrl();

  Serial.println("Open-Meteo:");
  Serial.println(url);

  if (!http.begin(client, url)) {
    snprintf(
      _status,
      sizeof(_status),
      "Errore HTTPS"
    );
    return false;
  }

  http.addHeader(
    "Accept",
    "application/json"
  );

  http.addHeader(
    "User-Agent",
    "WeatherStationPRO-Aurora/0005.4"
  );

  const int httpCode = http.GET();

  Serial.print("HTTP code: ");
  Serial.println(httpCode);

  Serial.print("Heap dopo GET: ");
  Serial.println(ESP.getFreeHeap());

  if (httpCode != HTTP_CODE_OK) {
    snprintf(
      _status,
      sizeof(_status),
      "Errore HTTP %d",
      httpCode
    );

    http.end();
    return false;
  }

  // Filtro ArduinoJson:
  // mantiene soltanto i campi realmente usati.
  DynamicJsonDocument filter(3072);

  filter["current"]["temperature_2m"] = true;
  filter["current"]["apparent_temperature"] = true;
  filter["current"]["relative_humidity_2m"] = true;
  filter["current"]["wind_speed_10m"] = true;
  filter["current"]["surface_pressure"] = true;
  filter["current"]["weather_code"] = true;
  filter["current"]["time"] = true;

  filter["daily"]["time"] = true;
  filter["daily"]["weather_code"] = true;
  filter["daily"]["temperature_2m_max"] = true;
  filter["daily"]["temperature_2m_min"] = true;
  filter["daily"]["precipitation_probability_max"] = true;
  filter["daily"]["sunrise"] = true;
  filter["daily"]["sunset"] = true;

  filter["hourly"]["time"] = true;
  filter["hourly"]["weather_code"] = true;
  filter["hourly"]["temperature_2m"] = true;
  filter["hourly"]["relative_humidity_2m"] = true;
  filter["hourly"]["wind_speed_10m"] = true;
  filter["hourly"]["precipitation_probability"] = true;

  // Il documento è allocato nell'heap.
  // Non viene più creata una String con tutto il JSON,
  // evitando la doppia occupazione di memoria.
  DynamicJsonDocument document(24576);

  Stream& responseStream = http.getStream();

  const DeserializationError error =
    deserializeJson(
      document,
      responseStream,
      DeserializationOption::Filter(filter)
    );

  http.end();

  Serial.print("Heap dopo JSON: ");
  Serial.println(ESP.getFreeHeap());

  if (error) {
    snprintf(
      _status,
      sizeof(_status),
      "Errore JSON: %s",
      error.c_str()
    );

    Serial.print("Errore JSON: ");
    Serial.println(error.c_str());

    return false;
  }

  if (document.overflowed()) {
    snprintf(
      _status,
      sizeof(_status),
      "JSON troppo grande"
    );

    Serial.println("ArduinoJson overflow");
    return false;
  }

  JsonObject current =
    document["current"].as<JsonObject>();

  JsonObject daily =
    document["daily"].as<JsonObject>();

  JsonObject hourly =
    document["hourly"].as<JsonObject>();

  if (
    current.isNull() ||
    daily.isNull() ||
    hourly.isNull()
  ) {
    snprintf(
      _status,
      sizeof(_status),
      "Dati mancanti"
    );
    return false;
  }

  _data.temperature =
    current["temperature_2m"] | 0.0f;

  _data.apparentTemperature =
    current["apparent_temperature"] | 0.0f;

  _data.humidity =
    current["relative_humidity_2m"] | 0;

  _data.windSpeed =
    current["wind_speed_10m"] | 0.0f;

  _data.pressure =
    current["surface_pressure"] | 0.0f;

  _data.weatherCode =
    current["weather_code"] | -1;

  extractTime(
    current["time"] | "",
    _data.updateTime,
    sizeof(_data.updateTime)
  );

  JsonArray dailyDates =
    daily["time"].as<JsonArray>();

  JsonArray dailyCodes =
    daily["weather_code"].as<JsonArray>();

  JsonArray dailyMax =
    daily["temperature_2m_max"].as<JsonArray>();

  JsonArray dailyMin =
    daily["temperature_2m_min"].as<JsonArray>();

  JsonArray dailyRain =
    daily[
      "precipitation_probability_max"
    ].as<JsonArray>();

  JsonArray sunrise =
    daily["sunrise"].as<JsonArray>();

  JsonArray sunset =
    daily["sunset"].as<JsonArray>();

  if (
    dailyDates.size() >= FORECAST_DAYS &&
    dailyCodes.size() >= FORECAST_DAYS &&
    dailyMax.size() >= FORECAST_DAYS &&
    dailyMin.size() >= FORECAST_DAYS &&
    dailyRain.size() >= FORECAST_DAYS
  ) {
    for (
      uint8_t index = 0;
      index < FORECAST_DAYS;
      index++
    ) {
      snprintf(
        _forecast[index].date,
        sizeof(_forecast[index].date),
        "%s",
        dailyDates[index] | "----------"
      );

      _forecast[index].weatherCode =
        dailyCodes[index] | -1;

      _forecast[index].maxTemperature =
        dailyMax[index] | 0.0f;

      _forecast[index].minTemperature =
        dailyMin[index] | 0.0f;

      _forecast[index].rainProbability =
        dailyRain[index] | 0;
    }

    _forecastValid = true;
  } else {
    clearForecast();
  }

  if (sunrise.size() > 0) {
    extractTime(
      sunrise[0] | "",
      _data.sunrise,
      sizeof(_data.sunrise)
    );
  }

  if (sunset.size() > 0) {
    extractTime(
      sunset[0] | "",
      _data.sunset,
      sizeof(_data.sunset)
    );
  }

  JsonArray hourlyTimes =
    hourly["time"].as<JsonArray>();

  JsonArray hourlyCodes =
    hourly["weather_code"].as<JsonArray>();

  JsonArray hourlyTemps =
    hourly["temperature_2m"].as<JsonArray>();

  JsonArray hourlyHumidity =
    hourly[
      "relative_humidity_2m"
    ].as<JsonArray>();

  JsonArray hourlyWind =
    hourly["wind_speed_10m"].as<JsonArray>();

  JsonArray hourlyRain =
    hourly[
      "precipitation_probability"
    ].as<JsonArray>();

  const size_t requiredHourly =
    static_cast<size_t>(FORECAST_DAYS) * 24U;

  if (
    hourlyTimes.size() >= requiredHourly &&
    hourlyCodes.size() >= requiredHourly &&
    hourlyTemps.size() >= requiredHourly &&
    hourlyHumidity.size() >= requiredHourly &&
    hourlyWind.size() >= requiredHourly &&
    hourlyRain.size() >= requiredHourly
  ) {
    for (
      uint8_t day = 0;
      day < FORECAST_DAYS;
      day++
    ) {
      for (
        uint8_t slot = 0;
        slot < HOURLY_SLOTS_PER_DAY;
        slot++
      ) {
        const size_t sourceIndex =
          static_cast<size_t>(day) * 24U +
          static_cast<size_t>(slot) * 3U;

        AuroraHourlySlot& target =
          _hourly[day][slot];

        extractTime(
          hourlyTimes[sourceIndex] | "",
          target.time,
          sizeof(target.time)
        );

        target.weatherCode =
          hourlyCodes[sourceIndex] | -1;

        target.temperature =
          hourlyTemps[sourceIndex] | 0.0f;

        target.humidity =
          hourlyHumidity[sourceIndex] | 0;

        target.windSpeed =
          hourlyWind[sourceIndex] | 0.0f;

        target.rainProbability =
          hourlyRain[sourceIndex] | 0;
      }
    }

    _hourlyValid = true;
  } else {
    clearHourly();
  }

  _data.valid = true;
  _lastUpdate = millis();

  snprintf(
    _status,
    sizeof(_status),
    "Meteo aggiornato"
  );

  Serial.println("Meteo aggiornato correttamente");

  return true;
}

bool AuroraWeather::shouldUpdate() const {
  if (!_data.valid) {
    return true;
  }

  return millis() - _lastUpdate >= WEATHER_UPDATE_INTERVAL_MS;
}

unsigned long AuroraWeather::lastUpdateMillis() const {
  return _lastUpdate;
}

const AuroraWeatherData& AuroraWeather::data() const {
  return _data;
}

const AuroraForecastDay& AuroraWeather::forecastDay(uint8_t index) const {
  if (index >= FORECAST_DAYS) {
    index = 0;
  }

  return _forecast[index];
}

const AuroraHourlySlot& AuroraWeather::hourlySlot(
  uint8_t dayIndex,
  uint8_t slotIndex
) const {
  if (dayIndex >= FORECAST_DAYS) {
    dayIndex = 0;
  }

  if (slotIndex >= HOURLY_SLOTS_PER_DAY) {
    slotIndex = 0;
  }

  return _hourly[dayIndex][slotIndex];
}

bool AuroraWeather::forecastValid() const {
  return _forecastValid;
}

bool AuroraWeather::hourlyValid() const {
  return _hourlyValid;
}

const char* AuroraWeather::description() const {
  return descriptionForCode(_data.weatherCode);
}

const char* AuroraWeather::descriptionForCode(int code) const {
  switch (code) {
    case 0: return "Sereno";
    case 1: return "Quasi sereno";
    case 2: return "Parz. nuvoloso";
    case 3: return "Nuvoloso";
    case 45:
    case 48: return "Nebbia";
    case 51:
    case 53:
    case 55: return "Pioviggine";
    case 56:
    case 57:
    case 61:
    case 63:
    case 65:
    case 66:
    case 67: return "Pioggia";
    case 71:
    case 73:
    case 75:
    case 77:
    case 85:
    case 86: return "Neve";
    case 80:
    case 81:
    case 82: return "Rovesci";
    case 95:
    case 96:
    case 99: return "Temporale";
    default: return "Non disponibile";
  }
}

const char* AuroraWeather::statusMessage() const {
  return _status;
}

String AuroraWeather::buildUrl() const {
  String url;
  url.reserve(800);

  url = "https://api.open-meteo.com/v1/forecast";

  const AuroraLocationSettings location =
    _settings.loadLocation();

  url += "?latitude=";
  url += String(location.latitude, 5);

  url += "&longitude=";
  url += String(location.longitude, 5);

  url +=
    "&current="
    "temperature_2m,"
    "relative_humidity_2m,"
    "apparent_temperature,"
    "weather_code,"
    "wind_speed_10m,"
    "surface_pressure";

  url +=
    "&daily="
    "weather_code,"
    "temperature_2m_max,"
    "temperature_2m_min,"
    "precipitation_probability_max,"
    "sunrise,"
    "sunset";

  url +=
    "&hourly="
    "temperature_2m,"
    "relative_humidity_2m,"
    "weather_code,"
    "wind_speed_10m,"
    "precipitation_probability";

  url += "&forecast_days=5";
  url += "&timezone=Europe%2FRome";

  return url;
}

void AuroraWeather::extractTime(
  const char* isoDateTime,
  char* output,
  size_t outputSize
) const {
  if (
    isoDateTime == nullptr ||
    strlen(isoDateTime) < 16
  ) {
    snprintf(output, outputSize, "--:--");
    return;
  }

  snprintf(output, outputSize, "%.5s", isoDateTime + 11);
}

void AuroraWeather::clearForecast() {
  for (uint8_t i = 0; i < FORECAST_DAYS; i++) {
    snprintf(_forecast[i].date, sizeof(_forecast[i].date), "----------");
    _forecast[i].weatherCode = -1;
    _forecast[i].minTemperature = 0.0f;
    _forecast[i].maxTemperature = 0.0f;
    _forecast[i].rainProbability = 0;
  }

  _forecastValid = false;
}

void AuroraWeather::clearHourly() {
  for (uint8_t day = 0; day < FORECAST_DAYS; day++) {
    for (uint8_t slot = 0; slot < HOURLY_SLOTS_PER_DAY; slot++) {
      AuroraHourlySlot& item = _hourly[day][slot];
      snprintf(item.time, sizeof(item.time), "--:--");
      item.weatherCode = -1;
      item.temperature = 0.0f;
      item.windSpeed = 0.0f;
      item.humidity = 0;
      item.rainProbability = 0;
    }
  }

  _hourlyValid = false;
}
