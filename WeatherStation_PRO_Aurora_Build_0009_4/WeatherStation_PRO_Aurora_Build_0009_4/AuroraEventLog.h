#pragma once
#include <Arduino.h>

enum AuroraEventLevel : uint8_t {
  EVENT_INFO = 0,
  EVENT_WARNING = 1,
  EVENT_ERROR = 2
};

struct AuroraEventEntry {
  unsigned long timestampSeconds;
  AuroraEventLevel level;
  char message[40];
};

class AuroraEventLog {
 public:
  static constexpr uint8_t CAPACITY = 12;
  AuroraEventLog();
  void begin();
  void add(AuroraEventLevel level, const char* message);
  uint8_t count() const;
  bool entry(uint8_t newestIndex, AuroraEventEntry& output) const;

 private:
  AuroraEventEntry _entries[CAPACITY];
  uint8_t _next;
  uint8_t _count;
};
