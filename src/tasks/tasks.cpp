#include "tasks.h"
#include "globals.h"

Tasks::Tasks() {}

void Tasks::init()
{
    SystemLog.addEvent(F("Initializing Watchdog Timer (WDG)"));
    esp_task_wdt_init(WDG_TIMEOUT, true);
    ESP_ERROR_CHECK(esp_task_wdt_add(NULL));
    esp_task_wdt_reset();
}

void Tasks::createSdCardHealthCheckTask()
{
    TaskHandle_t Task_SdCardHealthCheck;
    SystemLog.addEvent(F("Creating SD Card Health Check Task"));

    xTaskCreatePinnedToCore(
        [](void *pvParameters)
        {
            sd_card.cardCheckTask(pvParameters);
        },
        "SdCardHealthCheckTask",
        2048,
        NULL,
        1,
        &Task_SdCardHealthCheck,
        0);

    ESP_ERROR_CHECK(esp_task_wdt_add(Task_SdCardHealthCheck));
}

void Tasks::createTelemetryTask()
{
    TaskHandle_t Task_Telemetry;
    SystemLog.addEvent(F("Creating Telemetry Task"));

    xTaskCreatePinnedToCore(
        [](void *pvParameters)
        {
            SystemLog.addEvent(PSTR("Starting Telemetry task on core: ") + String(xPortGetCoreID()));

            TickType_t xLastWakeTime = xTaskGetTickCount();
            while (true)
            {
                esp_task_wdt_reset();
                SystemLog.addEvent(PSTR("Free RAM: ") + String(ESP.getFreeHeap()) + F(" B"));
                SystemLog.addEvent(PSTR("Min Free RAM: ") + String(ESP.getMinFreeHeap()) + F(" B"));
                SystemLog.addEvent(PSTR("Free PSRAM: ") + String(ESP.getFreePsram()) + F(" B"));
                SystemLog.addEvent(PSTR("Min Free PSRAM: ") + String(ESP.getMinFreePsram()) + F(" B"));
                SystemLog.addEvent(PSTR("MCU Temperature: ") + String(temperatureRead()) + F(" °C"));

                esp_task_wdt_reset();
                vTaskDelayUntil(&xLastWakeTime, TASK_TELEMETRY / portTICK_PERIOD_MS);
            }
        },
        "TelemetryTask",
        2048,
        NULL,
        2,
        &Task_Telemetry,
        0);

    ESP_ERROR_CHECK(esp_task_wdt_add(Task_Telemetry));
}

void Tasks::createTimelapseTask()
{
    TaskHandle_t Task_Timelapse;
    SystemLog.addEvent(F("Creating Timelapse Task"));

    xTaskCreatePinnedToCore(
        [](void *pvParameters)
        {
            SystemCamera.timelapseTask(pvParameters);
        },
        "TimelapseTask",
        4096,
        NULL,
        1,
        &Task_Timelapse,
        0);

    ESP_ERROR_CHECK(esp_task_wdt_add(Task_Timelapse));
}
