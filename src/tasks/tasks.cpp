#include "tasks.h"

Tasks::Tasks(Logger &i_logger) : logger(i_logger) {}

void Tasks::init()
{
    logger.addEvent(F("Initializing Watchdog Timer (WDG)"));
    esp_task_wdt_init(WDG_TIMEOUT, true);
    ESP_ERROR_CHECK(esp_task_wdt_add(NULL));
    esp_task_wdt_reset();
}

void Tasks::createSdCardHealthCheckTask(MicroSd *sdCard)
{
    TaskHandle_t Task_SdCardHealthCheck;
    logger.addEvent(F("Creating SD Card Health Check Task"));

    xTaskCreatePinnedToCore(
        [](void *pvParameters)
        {
            MicroSd *sdCard = static_cast<MicroSd *>(pvParameters);
            sdCard->cardCheckTask(pvParameters);
        },
        "SdCardHealthCheckTask",
        2048,
        sdCard,
        1,
        &Task_SdCardHealthCheck,
        0);

    ESP_ERROR_CHECK(esp_task_wdt_add(Task_SdCardHealthCheck));
}

void Tasks::createLedBlinkTask(BlinkingLed *flash_led)
{
    TaskHandle_t Task_LedBlink;
    logger.addEvent(F("Creating LED Blink Task"));

    xTaskCreatePinnedToCore(
        [](void *pvParameters)
        {
            BlinkingLed *flash_led = static_cast<BlinkingLed *>(pvParameters);
            flash_led->blinkingTask(pvParameters);
        },
        "LedBlinkTask",
        2048,
        flash_led,
        1,
        &Task_LedBlink,
        0);

    ESP_ERROR_CHECK(esp_task_wdt_add(Task_LedBlink));
}

void Tasks::createTelemetryTask()
{
    TaskHandle_t Task_Telemetry;
    logger.addEvent(F("Creating Telemetry Task"));

    xTaskCreatePinnedToCore(
        [](void *pvParameters)
        {
            Logger *logger = static_cast<Logger *>(pvParameters);
            logger->addEvent(PSTR("Starting Telemetry task on core: ") + String(xPortGetCoreID()));

            TickType_t xLastWakeTime = xTaskGetTickCount();
            while (true)
            {
                esp_task_wdt_reset();
                logger->addEvent(PSTR("Free RAM: ") + String(ESP.getFreeHeap()) + F(" B"));
                logger->addEvent(PSTR("Min Free RAM: ") + String(ESP.getMinFreeHeap()) + F(" B"));
                logger->addEvent(PSTR("Free PSRAM: ") + String(ESP.getFreePsram()) + F(" B"));
                logger->addEvent(PSTR("Min Free PSRAM: ") + String(ESP.getMinFreePsram()) + F(" B"));
                logger->addEvent(PSTR("MCU Temperature: ") + String(temperatureRead()) + F(" °C"));

                esp_task_wdt_reset();
                vTaskDelayUntil(&xLastWakeTime, TASK_TELEMETRY / portTICK_PERIOD_MS);
            }
        },
        "TelemetryTask",
        2048,
        &logger,
        2,
        &Task_Telemetry,
        0);

    ESP_ERROR_CHECK(esp_task_wdt_add(Task_Telemetry));
}

void Tasks::createTimelapseTask(Camera *camera)
{
    TaskHandle_t Task_Timelapse;
    logger.addEvent(F("Creating Timelapse Task"));

    xTaskCreatePinnedToCore(
        [](void *pvParameters)
        {
            Camera *camera = static_cast<Camera *>(pvParameters);
            camera->timelapseTask(pvParameters);
        },
        "TimelapseTask",
        4096,
        camera,
        1,
        &Task_Timelapse,
        0);

    ESP_ERROR_CHECK(esp_task_wdt_add(Task_Timelapse));
}
