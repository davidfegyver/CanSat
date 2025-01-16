#include <main.h>

void setup()
{
    SystemLog.addEvent(F("Setup started"));

#if (ENABLE_SERIAL_LOGS == true)
    Serial.begin(115200);
    Serial.setDebugOutput(true);

    SystemLog.addEvent(F("Serial logs enabled"));
#endif

#if (DISABLE_BROWNOUT)
    WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);

    SystemLog.addEvent(F("Brownout detector disabled"));
#endif

    EEPROM.begin(EEPROM_SIZE);

    sd_card.initCard();
    SystemLog.connectSdCard(&sd_card);
    SensorLog.connectSdCard(&sd_card);

    SystemCamera.init();
    tasks.init();
    tasks.createSdCardHealthCheckTask(&sd_card);
    tasks.createLedBlinkTask(&flash_led);
    tasks.createTelemetryTask();

    SystemLog.addEvent(F("Setup complete"));
}

void loop()
{
    esp_task_wdt_reset();
    delay(1000);
}
