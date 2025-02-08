#pragma once

#include <Arduino.h>
#include <esp_task_wdt.h>
#include "led.h"

class BlinkingLed : public Led
{
private:
    uint32_t offTime;
    uint32_t onTime;
    TaskHandle_t taskHandle;

public:
    BlinkingLed(uint8_t pin, uint8_t on_state, uint32_t off_time = 500, uint32_t on_time = 500);

    void setTimes(uint32_t off_time, uint32_t on_time);
    uint32_t getTimer();
    void blinkingTask(void *pvParameters);
    uint32_t getOffTime();
    uint32_t getOnTime();

    void createTask();
    void suspendTask();
    void resumeTask();
};
