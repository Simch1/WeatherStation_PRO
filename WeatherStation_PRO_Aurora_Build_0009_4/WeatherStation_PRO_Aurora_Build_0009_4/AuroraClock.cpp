#include "AuroraClock.h"
#include "AuroraConfig.h"
#include <WiFi.h>

AuroraClock::AuroraClock() : _valid(false) {}

void AuroraClock::begin() {
  configTzTime(
    TIMEZONE_ITALY,
    NTP_SERVER_1,
    NTP_SERVER_2,
    NTP_SERVER_3
  );
}

bool AuroraClock::synchronize(unsigned long timeoutMs) {
  if (WiFi.status() != WL_CONNECTED) {
    return false;
  }

  struct tm timeInfo;
  const unsigned long start = millis();

  while (
    !getLocalTime(&timeInfo, 500) &&
    millis() - start < timeoutMs
  ) {
    Serial.print(".");
  }

  Serial.println();
  _valid = getLocalTime(&timeInfo, 1000);
  return _valid;
}

void AuroraClock::update() {
  if (!_valid && WiFi.status() == WL_CONNECTED) {
    struct tm timeInfo;
    _valid = getLocalTime(&timeInfo, 20);
  }
}

bool AuroraClock::valid() const {
  return _valid;
}

void AuroraClock::getTime(
  char* output,
  size_t outputSize
) const {
  struct tm timeInfo;

  if (!getLocalTime(&timeInfo, 20)) {
    snprintf(output, outputSize, "--:--");
    return;
  }

  snprintf(
    output,
    outputSize,
    "%02d:%02d",
    timeInfo.tm_hour,
    timeInfo.tm_min
  );
}

void AuroraClock::getDate(
  char* output,
  size_t outputSize
) const {
  struct tm timeInfo;

  if (!getLocalTime(&timeInfo, 20)) {
    snprintf(output, outputSize, "--/--/----");
    return;
  }

  snprintf(
    output,
    outputSize,
    "%02d/%02d/%04d",
    timeInfo.tm_mday,
    timeInfo.tm_mon + 1,
    timeInfo.tm_year + 1900
  );
}
