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
    Tasks();

    void init();
    void createSdCardHealthCheckTask();
    void createDebugTask();
    void createTelemetryTask();
    void createTimelapseTask();
};
