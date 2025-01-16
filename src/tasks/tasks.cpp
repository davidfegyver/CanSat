#include "tasks.h"

Tasks::Tasks(Logger &i_logger) : logger(i_logger)
{}

void Tasks::init()
{
    logger.addEvent(F("Init WDG"));
    esp_task_wdt_init(WDG_TIMEOUT, true);
    ESP_ERROR_CHECK(esp_task_wdt_add(NULL));
    esp_task_wdt_reset();
}

void Tasks::createSdCardHealthCheckTask(MicroSd *sdCard)
{
    TaskHandle_t Task_SdCardHealthCheck;

    logger.addEvent("Creating SdCardHealthCheckTask");

    xTaskCreatePinnedToCore(
        [](void *pvParameters)
        {
            MicroSd *sdCard = (MicroSd *)pvParameters;
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

    logger.addEvent("Creating LedBlinkTask");

    xTaskCreatePinnedToCore(
        [](void *pvParameters)
        {
            BlinkingLed *flash_led = (BlinkingLed *)pvParameters;
            
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

    logger.addEvent("Creating TelemetryTask");
    xTaskCreatePinnedToCore(
        [](void *pvParameters)
        {
            Logger *logger = (Logger *)pvParameters;

            logger->addEvent(PSTR("Starting Telemetry task on core: ") + String(xPortGetCoreID()));
            TickType_t xLastWakeTime = xTaskGetTickCount();

            while (1)
            {
                esp_task_wdt_reset();
                logger->addEvent(PSTR("Free RAM: ") + String(ESP.getFreeHeap()) + PSTR(" B"));
                logger->addEvent(PSTR("Min Free RAM: ") + String(ESP.getMinFreeHeap()) + PSTR(" B"));
                logger->addEvent(PSTR("Free PSRAM: ") + String(ESP.getFreePsram()) + PSTR(" B"));
                logger->addEvent(PSTR("Min Free PSRAM: ") + String(ESP.getMinFreePsram()) + PSTR(" B"));
                logger->addEvent(PSTR("MCU Temperature: ") + String(temperatureRead()) + PSTR(" *C"));

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