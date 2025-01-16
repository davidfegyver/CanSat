#include "logging/logger.h"
#include "led/blinkingLed.h"
#include "micro_sd/micro_sd.h"
#include "logging/logger.h"
#include "camera/camera.h"
#include "esp_task_wdt.h"
#include "esp32-hal-cpu.h"

#include "micro_sd/micro_sd.h"


class Tasks;

class Tasks
{
public:
    Tasks(Logger &logger);
    ~Tasks() = default;

    void createSdCardHealthCheckTask(MicroSd *sdCard);
    void createLedBlinkTask(BlinkingLed *flash_led);
    void createTelemetryTask();
    void init();
    
private:
    Logger &logger;
};