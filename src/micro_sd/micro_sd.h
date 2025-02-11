#pragma once

#include <Arduino.h>
#include <FS.h>
#include <SD_MMC.h>
#include <esp_task_wdt.h>
#include "EspAsyncWebServer.h"
#include "cfg.h"
#include "ff.h"
#include "vfs_fat_internal.h"
#include "logging/logger.h"

class MicroSd
{
private:
  bool CardHealthy = false;
  uint32_t CardSizeMB = 0;
  uint32_t CardTotalMB = 0;
  uint32_t CardUsedMB = 0;
  uint32_t CardFreeMB = 0;
  uint8_t FreeSpacePercent = 0;
  uint8_t UsedSpacePercent = 100;
  uint8_t CardType;

  SemaphoreHandle_t sdCardMutex = xSemaphoreCreateMutex();

  void handleCardError(const String &);
  void handleCardType(uint8_t);

public:
  MicroSd();
  ~MicroSd() = default;

  void initCard();
  void reinitCard();
  void updateCardUsageStatus();
  void checkCardHealth();
  void formatCard();
  void cardCheckTask(void *pvParameters);

  bool openFile(File &, const String &);
  void closeFile(File &);
  bool isFileOpen(File &);

  bool createFile(const String &);
  bool removeFile(const String &);
  bool checkFile(const String &);
  bool renameFile(const String &, const String &);
  uint16_t fileCount(const String &, const String &);


  bool appendFile(File &, const String &);
  uint32_t getFileSize(const String &);

  bool createDir(const String &);
  bool removeDir(const String &);

  bool getCardHealthy();
  uint16_t getCardSizeMB();
  uint16_t getCardTotalMB();
  uint16_t getCardUsedMB();
  uint16_t getCardFreeMB();
  uint8_t getFreeSpacePercent();
  uint8_t getUsedSpacePercent();

  bool WritePicture(const String &, const uint8_t *, size_t);
  void sendFileToClient(AsyncWebServerRequest *, const String &);
};
