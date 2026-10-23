#include "AuroraPower.h"
#include "AuroraConfig.h"
#include <TFT_eSPI.h>

AuroraPower::AuroraPower(AuroraSettings& settings)
  : _settings(settings),
    _brightness(80),
    _standbySeconds(300),
    _lastActivity(0),
    _sleeping(false),
    _backlightPin(-1) {}

void AuroraPower::begin() {
#ifdef TFT_BL
  _backlightPin = TFT_BL;
  pinMode(_backlightPin, OUTPUT);
#endif

  _brightness = _settings.loadBrightness();
  _standbySeconds = _settings.loadStandbySeconds();
  _lastActivity = millis();
  applyBrightness(_brightness);
}

void AuroraPower::update() {
  if (_sleeping || _standbySeconds == 0) {
    return;
  }

  if (
    millis() - _lastActivity >=
    static_cast<unsigned long>(_standbySeconds) * 1000UL
  ) {
    applyBrightness(0);
    _sleeping = true;
  }
}

void AuroraPower::noteActivity() {
  _lastActivity = millis();
}

bool AuroraPower::wakeScreen() {
  if (!_sleeping) {
    noteActivity();
    return false;
  }

  _sleeping = false;
  _lastActivity = millis();
  applyBrightness(_brightness);
  return true;
}

bool AuroraPower::wakeAndConsumeTouch() {
  return wakeScreen();
}

bool AuroraPower::isSleeping() const {
  return _sleeping;
}

void AuroraPower::setBrightness(
  uint8_t percent,
  bool save
) {
  _brightness = constrain(percent, 20, 100);

  if (!_sleeping) {
    applyBrightness(_brightness);
  }

  if (save) {
    _settings.saveBrightness(_brightness);
  }
}

uint8_t AuroraPower::brightness() const {
  return _brightness;
}

void AuroraPower::setStandbySeconds(
  uint32_t seconds,
  bool save
) {
  _standbySeconds = seconds;
  noteActivity();

  if (save) {
    _settings.saveStandbySeconds(seconds);
  }
}

uint32_t AuroraPower::standbySeconds() const {
  return _standbySeconds;
}

bool AuroraPower::backlightAvailable() const {
  return _backlightPin >= 0;
}

void AuroraPower::applyBrightness(uint8_t percent) {
  if (_backlightPin < 0) {
    return;
  }

  uint8_t duty = map(constrain(percent, 0, 100), 0, 100, 0, 255);

  if (!AURORA_BACKLIGHT_ACTIVE_HIGH) {
    duty = 255 - duty;
  }

  analogWrite(_backlightPin, duty);
}
