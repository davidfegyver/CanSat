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

void Tasks::createDebugTask()
{
    TaskHandle_t Task_Debug;
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
                vTaskDelayUntil(&xLastWakeTime, TASK_DEBUG / portTICK_PERIOD_MS);
            }
        },
        "DebugTask",
        2048,
        NULL,
        2,
        &Task_Debug,
        0);

    ESP_ERROR_CHECK(esp_task_wdt_add(Task_Debug));
}

void Tasks::createTimelapseTask()
{
    TaskHandle_t Task_Telemetry;
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
        &Task_Telemetry,
        0);

    ESP_ERROR_CHECK(esp_task_wdt_add(Task_Telemetry));
}

void Tasks::createTelemetryTask()
{

    TaskHandle_t Task_Telemetry;
    SystemLog.addEvent(F("Creating Timelapse Task"));

    xTaskCreatePinnedToCore(
        [](void *pvParameters)
        {
            SystemLog.addEvent(PSTR("Starting telemetry task on core: ") + String(xPortGetCoreID()));

            TickType_t xLastWakeTime = xTaskGetTickCount();

            while (true)
            {
                esp_task_wdt_reset();

                sensors_event_t lastAccelerometerEvent = i2c.getLastAccelerometerEvent();
                sensors_event_t lastGyroscopeEvent = i2c.getLastGyroscopeEvent();
                sensors_event_t lastMagnetometerEvent = i2c.getLastMagnetometerEvent();
                float lastTemperature = i2c.getLastTemperature();
                float lastPressure = i2c.getLastPressure();

                String lastHash = SystemCamera.getLastHash();

                String csv = String(lastTemperature) + "," +
                             String(lastPressure) + "," +
                             String(lastAccelerometerEvent.acceleration.x) + "," +
                             String(lastAccelerometerEvent.acceleration.y) + "," +
                             String(lastAccelerometerEvent.acceleration.z) + "," +
                             String(lastGyroscopeEvent.gyro.x) + "," +
                             String(lastGyroscopeEvent.gyro.y) + "," +
                             String(lastGyroscopeEvent.gyro.z) + "," +
                             String(lastMagnetometerEvent.magnetic.x) + "," +
                             String(lastMagnetometerEvent.magnetic.y) + "," +
                             String(lastMagnetometerEvent.magnetic.z) + "," +
                                lastHash;

                lora.radioTransmit(csv, 5);

                esp_task_wdt_reset();
                vTaskDelayUntil(&xLastWakeTime, TASK_TELEMETRY / portTICK_PERIOD_MS);
            }
        },
        "TelemetryTask",
        4096,
        NULL,
        1,
        &Task_Telemetry,
        0);

    ESP_ERROR_CHECK(esp_task_wdt_add(Task_Telemetry));
}