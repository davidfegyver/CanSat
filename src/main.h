#pragma once

#include "Arduino.h"

#include <EEPROM.h>

#include "led/blinkingLed.h"
#include "micro_sd/micro_sd.h"
#include "logging/logger.h"

#include "esp32/rom/rtc.h"
#include <Wifi.h>

#include "esp_task_wdt.h"
#include "esp32-hal-cpu.h"

#include "camera/camera.h"


#include "cfg.h"

Logger SystemLog(LOGS_FILE_PATH, SYSTEMLOG_FILENAME);
Logger SensorLog(LOGS_FILE_PATH, SENSORLOG_FILENAME);

BlinkingLed flash_led(SystemLog, FLASH_GPIO, FLASH_ON_STATUS, 200, 500);
MicroSd sd_card(SystemLog);
Camera SystemCamera(SystemLog);

TaskHandle_t Task_FlashLed;
TaskHandle_t Task_SdCardHealthCheck;