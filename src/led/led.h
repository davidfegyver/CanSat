#pragma once

#include <Arduino.h>
#include "logging/logger.h"

class Led
{
protected:
  uint8_t pin;
  uint8_t onState;
  Logger &logger;

public:
  Led(Logger &logger, uint8_t pin, uint8_t on_state);
  ~Led() = default;

  void toggle();
  void set(bool state);
  bool get();
  void turnOn();
  void turnOff();
};
