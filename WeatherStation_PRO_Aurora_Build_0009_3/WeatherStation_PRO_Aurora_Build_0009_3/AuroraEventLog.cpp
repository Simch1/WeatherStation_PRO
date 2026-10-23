#include "AuroraEventLog.h"

AuroraEventLog::AuroraEventLog()
  : _next(0), _count(0) {}

void AuroraEventLog::begin() {
  _next = 0;
  _count = 0;
  for (uint8_t i = 0; i < CAPACITY; i++) {
    _entries[i].timestampSeconds = 0;
    _entries[i].level = EVENT_INFO;
    _entries[i].message[0] = '\0';
  }
}

void AuroraEventLog::add(
  AuroraEventLevel level,
  const char* message
) {
  AuroraEventEntry& target = _entries[_next];
  target.timestampSeconds = millis() / 1000UL;
  target.level = level;
  snprintf(
    target.message,
    sizeof(target.message),
    "%s",
    message != nullptr ? message : ""
  );
  _next = (_next + 1) % CAPACITY;
  if (_count < CAPACITY) _count++;
}

uint8_t AuroraEventLog::count() const {
  return _count;
}

bool AuroraEventLog::entry(
  uint8_t newestIndex,
  AuroraEventEntry& output
) const {
  if (newestIndex >= _count) return false;

  int index =
    static_cast<int>(_next) -
    1 -
    static_cast<int>(newestIndex);

  while (index < 0) index += CAPACITY;

  output = _entries[index];
  return true;
}
