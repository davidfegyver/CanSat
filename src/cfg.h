/* -------- Camera config -------- */
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

/* -------- MCU config -------- */
#define DISABLE_BROWNOUT true

/* -------- OTA config -------- */

/* -------- SD card config -------- */

/*

1 line SD mode, hogy tudjuk használni a beépített ledet és több GPIO-t
https://github.com/espressif/arduino-esp32/blob/master/libraries/SD_MMC/README.md#4-line-and-1-line-sd-modes

*/
#define IS_SD_ONE_LINE true

/* -------- Built-in LED GPIO config -------- */
#define FLASH_GPIO 4
#define FLASH_ON_STATUS 205 // Limit flash intensity to 80%. 2^8*0.8

#define STATUS_GPIO 33

/* -------- WIFI config -------- */

#define WIFI_SSID "CanBus - CanSat"
#define WIFI_PASSWORD "HyperSecure"
#define WIFI_CHANNEL 1
#define WIFI_MAX_CLIENTS 1
#define WIFI_IP {1, 3, 3, 7}
#define WIFI_SUBNET {255, 255, 255, 0}
#define DEVICE_HOSTNAME "CanSat"

/* -------- Photos config -------- */

#define PHOTO_FOLDER "/photos"
#define PHOTO_PREFIX "photo"
#define PHOTO_SUFFIX ".jpg"

/* -------- EEPROM config -------- */

#define EEPROM_ADDR_SOMETHING_I_CAN_FEEL 0
#define EEPROM_LENGTH_SOMETHING_I_CAN_FEEL 1

#define EEPROM_SIZE (EEPROM_LENGTH_SOMETHING_I_CAN_FEEL)

/* -------- MicroSD Logs -------- */
#define SYSTEMLOG_FILENAME "System.log"
#define SENSORLOG_FILENAME "Sensors.log"
#define LOGS_FILE_PATH "/logs"
#define LOGS_MAX_SIZE 1024
#define ENABLE_SERIAL_LOGS true

/* -------- Webserver config -------- */
#define WEB_SERVER_PORT 8080
#define WEB_CACHE_INTERVAL 60000

/* -------- Tasks -------- */

#define TASK_TELEMETRY 10000
#define WDG_TIMEOUT 30000
#define TASK_SDCARD 10000
#define TASK_TIMELAPSE 500

/* -------- Basic config -------- */
#define SW_BUILD __DATE__ " " __TIME__
#define SERIAL_BAUDRATE 115200

/* ------- LoRa config -------- */
#define LORA_BAUDRATE 115200
#define LORA_RX 12
#define LORA_TX 13
#define LORA_FREQ_MHZ 868.5
#define LORA_POWER 20
#define LORA_PABOOST "off"
#define LORA_CRC "off"