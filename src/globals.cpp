#include "globals.h"

Logger SystemLog(LOGS_FILE_PATH, SYSTEMLOG_FILENAME);
Logger SensorLog(LOGS_FILE_PATH, SENSORLOG_FILENAME);
MicroSd sd_card;
BlinkingLed flash_led(FLASH_GPIO, FLASH_ON_STATUS, 200, 500);

Camera SystemCamera;
Network network;
Lora lora(Serial1);
I2C i2c;

Tasks tasks;
