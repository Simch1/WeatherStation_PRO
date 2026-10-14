#pragma once

#include <Arduino.h>
#include "AuroraSettings.h"

constexpr uint8_t FORECAST_DAYS = 5;
constexpr uint8_t HOURLY_SLOTS_PER_DAY = 8;

struct AuroraWeatherData {
  float temperature;
  float apparentTemperature;
  float windSpeed;
  float pressure;
  int humidity;
  int weatherCode;

  char sunrise[6];
  char sunset[6];
  char updateTime[6];

  bool valid;
};

struct AuroraForecastDay {
  char date[11];
  int weatherCode;
  float minTemperature;
  float maxTemperature;
  int rainProbability;
};

struct AuroraHourlySlot {
  char time[6];
  int weatherCode;
  float temperature;
  float windSpeed;
  int humidity;
  int rainProbability;
};

class AuroraWeather {
 public:
  explicit AuroraWeather(AuroraSettings& settings);

  void begin();
  bool update(bool networkAvailable);
  bool shouldUpdate() const;
  unsigned long lastUpdateMillis() const;

  const AuroraWeatherData& data() const;
  const AuroraForecastDay& forecastDay(uint8_t index) const;
  const AuroraHourlySlot& hourlySlot(uint8_t dayIndex, uint8_t slotIndex) const;

  bool forecastValid() const;
  bool hourlyValid() const;

  const char* description() const;
  const char* descriptionForCode(int code) const;
  const char* statusMessage() const;

 private:
  AuroraSettings& _settings;
  AuroraWeatherData _data;
  AuroraForecastDay _forecast[FORECAST_DAYS];
  AuroraHourlySlot _hourly[FORECAST_DAYS][HOURLY_SLOTS_PER_DAY];

  bool _forecastValid;
  bool _hourlyValid;

  unsigned long _lastUpdate;
  char _status[48];

  String buildUrl() const;

  void extractTime(
    const char* isoDateTime,
    char* output,
    size_t outputSize
  ) const;

  void clearForecast();
  void clearHourly();
};
