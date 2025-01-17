#pragma once

#include "logging/logger.h"
#include "led/blinkingLed.h"
#include "micro_sd/micro_sd.h"
#include "camera/camera.h"
#include "esp_task_wdt.h"
#include "esp32-hal-cpu.h"

class Tasks
{
public:
    explicit Tasks(Logger &logger);
    ~Tasks() = default;

    void init();
    void createSdCardHealthCheckTask(MicroSd *sdCard);
    void createLedBlinkTask(BlinkingLed *flash_led);
    void createTelemetryTask();
    void createTimelapseTask(Camera *camera);

private:
    Logger &logger;
};
