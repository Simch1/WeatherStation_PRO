#pragma once

#include <Arduino.h>
#include "AuroraSettings.h"

class AuroraPower {
 public:
  explicit AuroraPower(AuroraSettings& settings);

  void begin();
  void update();

  void noteActivity();
  bool wakeScreen();
  bool wakeAndConsumeTouch();
  bool isSleeping() const;

  void setBrightness(uint8_t percent, bool save);
  uint8_t brightness() const;

  void setStandbySeconds(uint32_t seconds, bool save);
  uint32_t standbySeconds() const;

  bool backlightAvailable() const;

 private:
  AuroraSettings& _settings;

  uint8_t _brightness;
  uint32_t _standbySeconds;
  unsigned long _lastActivity;
  bool _sleeping;
  int _backlightPin;

  void applyBrightness(uint8_t percent);
};
