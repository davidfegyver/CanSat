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


/* -------- Photos config -------- */


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


/* -------- Tasks -------- */


#define WDG_TIMEOUT 40000
#define TASK_SDCARD 25000


/* -------- Basic config -------- */
#define SW_BUILD __DATE__ " " __TIME__
#define SERIAL_BAUDRATE 115200