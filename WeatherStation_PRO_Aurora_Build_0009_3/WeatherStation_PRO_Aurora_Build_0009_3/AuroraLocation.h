#pragma once

#include <Arduino.h>
#include "AuroraSettings.h"

class AuroraLocation {
 public:
  explicit AuroraLocation(AuroraSettings& settings);

  bool resolvePendingCity(bool networkAvailable);
  const char* statusMessage() const;

 private:
  AuroraSettings& _settings;
  char _status[64];

  String encodeUrl(const String& input) const;
};
