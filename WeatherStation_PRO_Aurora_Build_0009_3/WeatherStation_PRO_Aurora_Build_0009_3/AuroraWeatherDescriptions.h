#pragma once

#include <Arduino.h>

#include "AuroraLanguage.h"

const char* auroraWeatherDescription(
  int weatherCode,
  AuroraLanguageId language
);
