#include <main.h>

void setup()
{
#if (ENABLE_SERIAL_LOGS == true)
    Serial.begin(115200);
    Serial.setDebugOutput(true);
#endif

#if (DISABLE_BROWNOUT)
    WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);
#endif

    EEPROM.begin(EEPROM_SIZE);

    sd_card.initCard();
    SystemLog.connectSdCard(&sd_card);
    SensorLog.connectSdCard(&sd_card);

    SystemCamera.init();

    SystemLog.addEvent(F("Init WDG"));

    esp_task_wdt_init(WDG_TIMEOUT, true);

    ESP_ERROR_CHECK(esp_task_wdt_add(NULL));
    esp_task_wdt_reset();

    SystemLog.addEvent("Starting Tasks");

    if (sd_card.getCardHealthy())
    {
        SystemLog.addEvent("Creating SdCardHealthCheckTask");
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

    /*     SystemLog.addEvent("Creating FlashLedTask");
        xTaskCreatePinnedToCore(
            [](void *pvParameters)
            {
                flash_led.blinkingTask(pvParameters);
            },
            "FlashLedTask",
            2048,
            NULL,
            2,
            &Task_FlashLed,
            0);
        ESP_ERROR_CHECK(esp_task_wdt_add(Task_FlashLed));
     */

    SystemLog.addEvent("Creating TelemetryTask");
    xTaskCreatePinnedToCore(
        [](void *pvParameters)
        {
            SystemLog.addEvent(PSTR("Starting Telemetry task on core: ") + String(xPortGetCoreID()));
            TickType_t xLastWakeTime = xTaskGetTickCount();

            while (1)
            {
                esp_task_wdt_reset();
                SystemLog.addEvent(PSTR("Free RAM: ") + String(ESP.getFreeHeap()) + PSTR(" B"));
                SystemLog.addEvent(PSTR("Min Free RAM: ") + String(ESP.getMinFreeHeap()) + PSTR(" B"));
                SystemLog.addEvent(PSTR("Free PSRAM: ") + String(ESP.getFreePsram()) + PSTR(" B"));
                SystemLog.addEvent(PSTR("Min Free PSRAM: ") + String(ESP.getMinFreePsram()) + PSTR(" B"));
                SystemLog.addEvent(PSTR("MCU Temperature: ") + String(temperatureRead()) + PSTR(" *C"));

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

    SystemLog.addEvent("Setup complete");
}

void loop()
{
    esp_task_wdt_reset();
    delay(1000);
}
