#pragma once

#include <Arduino.h>
#include <esp_task_wdt.h>
#include "Led.h"
#include "../logging/logger.h"

class BlinkingLed : public Led
{
private:
    uint32_t offTime;
    uint32_t onTime;

public:
    BlinkingLed(Logger &, uint8_t, uint8_t, uint32_t, uint32_t);
    ~BlinkingLed() {};

    void setTimes(uint32_t, uint32_t);
    uint32_t getTimer();
    void blinkingTask(void *);
    uint32_t getOffTime();
    uint32_t getOnTime();
};
