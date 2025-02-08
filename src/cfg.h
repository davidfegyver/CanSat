// cfg.h - Configuration file for CanSat

#pragma once

#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>

/* -------- Camera Configuration -------- */
#define PWDN_GPIO_NUM 32
#define RESET_GPIO_NUM -1
#define XCLK_GPIO_NUM 0
#define SIOD_GPIO_NUM 26
#define SIOC_GPIO_NUM 27
#define Y9_GPIO_NUM 35
#define Y8_GPIO_NUM 34
#define Y7_GPIO_NUM 39
#define Y6_GPIO_NUM 36
#define Y5_GPIO_NUM 21
#define Y4_GPIO_NUM 19
#define Y3_GPIO_NUM 18
#define Y2_GPIO_NUM 5
#define VSYNC_GPIO_NUM 25
#define HREF_GPIO_NUM 23
#define PCLK_GPIO_NUM 22

#define PHOTO_QUALITY 10
#define FRAME_SIZE 0
#define MAX_CAMERA_FAILS 5

/* -------- Microcontroller Configuration -------- */
#define DISABLE_BROWNOUT true

/* -------- SD Card Configuration -------- */

/*

1 line SD mode, hogy tudjuk használni a beépített ledet és több GPIO-t
https://github.com/espressif/arduino-esp32/blob/master/libraries/SD_MMC/README.md#4-line-and-1-line-sd-modes

*/
#define IS_SD_ONE_LINE true

/* -------- Built-in LED Configuration -------- */
#define FLASH_GPIO 4
#define FLASH_ON_STATUS 255 // 2^8*percent
#define STATUS_GPIO 33

/* -------- WiFi Configuration -------- */

#define WIFI_SSID "CanBus - CanSat"
#define WIFI_PASSWORD "HyperSecure"
#define WIFI_CHANNEL 1
#define WIFI_MAX_CLIENTS 1
#define WIFI_IP {1, 3, 3, 7}
#define WIFI_SUBNET {255, 255, 255, 0}
#define DEVICE_HOSTNAME "CanSat"

/* -------- Photo Storage Configuration -------- */

#define PHOTO_FOLDER "/photos"
#define PHOTO_PREFIX "photo"
#define PHOTO_SUFFIX ".jpg"

/* -------- EEPROM config -------- */

#define EEPROM_ADDR_SOMETHING_I_CAN_FEEL 0
#define EEPROM_LENGTH_SOMETHING_I_CAN_FEEL 1

#define EEPROM_SIZE (EEPROM_LENGTH_SOMETHING_I_CAN_FEEL)

/* -------- EEPROM Configuration -------- */
#define SYSTEMLOG_FILENAME "System.log"
#define SENSORLOG_FILENAME "Sensors.log"
#define LOGS_FILE_PATH "/logs"
#define LOGS_MAX_SIZE 1024
#define ENABLE_SERIAL_LOGS true

/* -------- Web Server Configuration -------- */
#define WEB_SERVER_PORT 8080
#define WEB_CACHE_INTERVAL 60000

/* -------- Task Timings (in ms) -------- */

#define WDG_TIMEOUT 30000

#define TASK_TELEMETRY 10000
#define TASK_SDCARD 10000
#define TASK_TIMELAPSE 500
#define TASK_MPU6050 100
#define TASK_HMC5883L 100
#define TASK_MS5611 100

/* -------- System Configuration -------- */
#define SW_BUILD __DATE__ " " __TIME__
#define SERIAL_BAUD 115200

/* -------- LoRa Configuration -------- */
#define LORA_TX 13

#define LORA_BAUD 115200
#define LORA_FREQ_MHZ 868.5
#define LORA_POWER 20
#define LORA_PABOOST "off"
#define LORA_CRC "off"
#define LORA_BANDWIDTH 250

/* -------- I2C Configuration -------- */
#define I2C_SDA 13
#define I2C_SCL 16

/* -------- Sensor Configuration -------- */
#define GYRO_RANGE MPU6050_RANGE_500_DEG
#define ACCEL_RANGE MPU6050_RANGE_2_G

// #define COMPASS_RANGE HMC5883L_RANGE_1_3GA
