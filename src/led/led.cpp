#include "led.h"

Led::Led(Logger &i_logger, uint8_t i_pin, uint8_t i_on_state)
    : pin(i_pin), onState(i_on_state), logger(i_logger)
{
  pinMode(pin, OUTPUT);
  turnOff();
  logger.addEvent(PSTR("Led initialized with pin: ") + String(i_pin) +
                  PSTR(", on state: ") + String(i_on_state));
}

void Led::toggle()
{
  if (get() == LOW)
  {
    turnOn();
  }
  else
  {
    turnOff();
  }
}

void Led::turnOn()
{
  set(onState);
}

void Led::turnOff()
{
  set(LOW);
}

void Led::set(bool state)
{
  digitalWrite(pin, state);
}

bool Led::get()
{
  return digitalRead(pin);
}
