#pragma once

#include <Arduino.h>
#include <FS.h>
#include <SD_MMC.h>
#include <esp_task_wdt.h>

#include "cfg.h"

class Logger;

class MicroSd
{
private:
  bool CardHealthy = false;
  bool CardHealthyOnBoot = false;
  uint32_t CardSizeMB = 0;
  uint32_t CardTotalMB = 0;
  uint32_t CardUsedMB = 0;
  uint32_t CardFreeMB = 0;
  uint8_t FreeSpacePercent = 0;
  uint8_t UsedSpacePercent = 100;
  uint8_t CardType;

  Logger &logger;
  SemaphoreHandle_t sdCardMutex = xSemaphoreCreateMutex();

public:
  MicroSd(Logger &);
  ~MicroSd() {};

  void initCard();
  void reinitCard();

  bool openFile(File *, String);
  void closeFile(File *);
  bool isFileOpen(File *);

  bool createDir(String);
  bool removeDir(String);
  bool checkFile(String);
  bool removeFile(String);
  bool createFile(String);
  bool createFileIfNotExists(String);

  bool appendFile(File *, String *);
  bool renameFile(String, String);
  bool deleteFile(String);
  uint32_t getFileSize(String);
  uint16_t fileCount(String, String);
  bool removeFilesInDir(String, int);
  int countFilesInDir(String);

  void updateCardUsageStatus();
  void checkCardHealth();

  bool getCardHealthy();

  void cardCheckTask(void *pvParameters);

  uint16_t getCardSizeMB();
  uint16_t getCardTotalMB();
  uint16_t getCardUsedMB();
  uint16_t getCardFreeMB();
  uint8_t getFreeSpacePercent();
  uint8_t getUsedSpacePercent();
};