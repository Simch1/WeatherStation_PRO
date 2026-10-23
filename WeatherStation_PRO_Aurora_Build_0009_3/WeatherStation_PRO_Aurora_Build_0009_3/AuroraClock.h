#pragma once

#include <Arduino.h>
#include <time.h>

class AuroraClock {
 public:
  AuroraClock();

  void begin();
  bool synchronize(unsigned long timeoutMs);
  void update();

  bool valid() const;
  void getTime(char* output, size_t outputSize) const;
  void getDate(char* output, size_t outputSize) const;

 private:
  bool _valid;
};
