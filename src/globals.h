#pragma once

#include <Arduino.h>
#include <EEPROM.h>
#include <Wifi.h>

#include "cfg.h"
#include "esp32/rom/rtc.h"
#include "esp_task_wdt.h"
#include "esp32-hal-cpu.h"

#include "logging/logger.h"
#include "micro_sd/micro_sd.h"
#include "led/blinkingLed.h"

#include "camera/camera.h"
#include "network/network.h"
#include "lora/lora.h"
#include "i2c/i2c.h"

#include "tasks/tasks.h"

extern Logger SystemLog;
extern Logger SensorLog;
extern MicroSd sd_card;
extern BlinkingLed flash_led;

extern Camera SystemCamera;
extern Network network;

extern Lora lora;
extern I2C i2c;

extern Tasks tasks;
