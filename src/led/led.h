#pragma once

#include <Arduino.h>

class Led
{
protected:
  uint8_t pin;
  uint8_t onState;

public:
  Led(uint8_t pin, uint8_t on_state);

  void toggle();
  void set(bool state);
  bool get();
  void turnOn();
  void turnOff();
};
