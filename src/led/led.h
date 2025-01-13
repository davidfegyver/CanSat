#pragma once

#include <Arduino.h>
#include "../logging/logger.h"

class Led
{
protected:
  uint8_t pin;
  uint8_t onState;
  Logger &logger;

public:
  Led(Logger &, uint8_t, uint8_t);
  ~Led() {};

  void toggle();
  void set(bool);
  bool get();
  void turnOn();
  void turnOff();
};
