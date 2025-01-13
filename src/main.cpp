#include <main.h>

void setup()
{
#if (ENABLE_SERIAL_LOGS == true)
    Serial.begin(115200);
    Serial.setDebugOutput(true);
#endif

    EEPROM.begin(EEPROM_SIZE);

    sd_card.initCard();
    SystemLog.connectSdCard(&sd_card);
    SensorLog.connectSdCard(&sd_card);

    SystemLog.AddEvent(F("Init WDG"));

    esp_task_wdt_init(WDG_TIMEOUT, true);

    ESP_ERROR_CHECK(esp_task_wdt_add(NULL));
    esp_task_wdt_reset();

    SystemLog.AddEvent("Starting Tasks");

    if (sd_card.getCardHealthy())
    {
        SystemLog.AddEvent("Creating SdCardHealthCheckTask");
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

    SystemLog.AddEvent("Creating FlashLedTask");
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

    SystemLog.AddEvent("Setup complete");
}

void loop()
{
    esp_task_wdt_reset();
    delay(1000);
}
