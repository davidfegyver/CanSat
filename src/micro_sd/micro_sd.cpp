#include "micro_sd.h"
#include "logging/logger.h"

MicroSd::MicroSd(Logger &i_logger) : logger(i_logger) {}

void MicroSd::initCard()
{
  CardHealthy = true;
  CardHealthyOnBoot = true;

  logger.AddEvent(F("Start init micro-SD Card"));

  if (!SD_MMC.begin("/sdcard", IS_SD_ONE_LINE))
  {
    logger.AddEvent(F("SD Card Mount Failed"));
    CardHealthyOnBoot = false;
    CardHealthy = false;
    return;
  }


  CardType = SD_MMC.cardType();

  logger.AddEvent(PSTR("SD Card Type: ") + String(CardType));
  switch (CardType)
  {
  case CARD_MMC:
    logger.AddEvent(F("MMC"));
    break;
  case CARD_SD:
    logger.AddEvent(F("SDSC"));
    break;
  case CARD_SDHC:
    logger.AddEvent(F("SDHC"));
    break;
  default:
    logger.AddEvent(F("No/unknown SD Card attached!"));
    CardHealthyOnBoot = false;
    CardHealthy = false;
    return;
  }

  checkCardHealth();

  if (CardHealthy)
  {
    logger.AddEvent(F("SD Card initialized successfully!"));
  }
  else
  {
    CardHealthyOnBoot = false;
    logger.AddEvent(F("SD Card initialization failed!"));
  }
}

void MicroSd::reinitCard()
{
  logger.AddEvent(F("Reinit micro SD card!"));
  logger.AddEvent(F("Deinit micro SD card"));
  SD_MMC.end();
  delay(50);
  logger.AddEvent(F("Init micro SD card"));
  initCard();
}

void MicroSd::updateCardUsageStatus()
{
  if (!CardHealthy)
    return;

  CardSizeMB = SD_MMC.cardSize() / (1024 * 1024);
  CardTotalMB = SD_MMC.totalBytes() / (1024 * 1024);
  CardUsedMB = SD_MMC.usedBytes() / (1024 * 1024);
  CardFreeMB = CardSizeMB - CardUsedMB;
  FreeSpacePercent = (CardFreeMB * 100) / CardSizeMB;
  UsedSpacePercent = 100 - FreeSpacePercent;

  logger.AddEvent(PSTR("Card usage - Size: ") + String(CardSizeMB) + F(" MB, Used: ") + String(CardUsedMB) + F(" MB, Free: ") + String(CardFreeMB) + F(" MB, Free: ") + String(FreeSpacePercent) + F("%"));
}

void MicroSd::checkCardHealth()
{
  if (!CardHealthy)
    return;

  updateCardUsageStatus();

  if (CardSizeMB == 0 || CardTotalMB == 0 || FreeSpacePercent <= 5)
  {
    logger.AddEvent(F("SD Card health check failed: Low space or invalid size"));
    CardHealthy = false;
  }
}

void MicroSd::cardCheckTask(void *pvParameters)
{
  logger.AddEvent(PSTR("Starting SD card health check task on core ") + String(xPortGetCoreID()));
  TickType_t xLastWakeTime = xTaskGetTickCount();

  while (true)
  {
    esp_task_wdt_reset();
    checkCardHealth();

    logger.AddEvent(PSTR("Card healthy status: ") + String(CardHealthy));

    if (!CardHealthy && CardHealthyOnBoot)
    {
      reinitCard();
    }

    esp_task_wdt_reset();
    vTaskDelayUntil(&xLastWakeTime, TASK_SDCARD / portTICK_PERIOD_MS);
  }
}

bool MicroSd::openFile(File *i_file, String i_path)
{
  if (!CardHealthy)
  {
    return false;
  }

  *i_file = SD_MMC.open(i_path.c_str(), FILE_APPEND);
  if (!*i_file)
  {
    CardHealthy = false;
    return false;
  }
  return true;
}

void MicroSd::closeFile(File *i_file)
{
  if (*i_file)
  {
    i_file->close();
  }
}

bool MicroSd::isFileOpen(File *i_file)
{
  return *i_file;
}

bool MicroSd::renameFile(String path1, String path2)
{
  if (!CardHealthy)
  {
    return false;
  }

  logger.AddEvent(PSTR("Renaming file: ") + path1 + PSTR(" to ") + path2);

  return SD_MMC.rename(path1.c_str(), path2.c_str());
}

bool MicroSd::deleteFile(String path)
{
  if (!CardHealthy)
  {
    return false;
  }

  logger.AddEvent(PSTR("Deleting file: ") + path);
  return SD_MMC.remove(path.c_str());
}
uint32_t MicroSd::getFileSize(String path)
{
  if (!CardHealthy)
    return 0;

  File file = SD_MMC.open(path.c_str(), FILE_APPEND);
  if (!file)
  {
    return 0;
  }
  uint32_t size = file.size();

  file.close();

  return size / 1024;
}

bool MicroSd::createDir(String path)
{
    if (!CardHealthy)
  {
    return false;
  }

  logger.AddEvent(PSTR("CreateDir: ") + path);
  return SD_MMC.mkdir(path.c_str());
}

bool MicroSd::removeDir(String path)
{
    if (!CardHealthy)
  {
    return false;
  }

  logger.AddEvent(PSTR("RemoveDir: ") + path);
  return SD_MMC.rmdir(path.c_str());
}

bool MicroSd::checkFile(String path)
{
  if (!CardHealthy)
  {
    return false;
  }

  logger.AddEvent(PSTR("Checking if file/directory exists: ") + path);

  return  SD_MMC.exists(path.c_str());
}

bool MicroSd::createFile(String path)
{
  if (!CardHealthy)
  {
    return false;
  }

  logger.AddEvent(PSTR("CreateFile: ") + path);
  return SD_MMC.open(path.c_str(), FILE_WRITE);
}

bool MicroSd::removeFile(String path)
{
  if (!CardHealthy)
  {
    return false;
  }
  return  SD_MMC.remove(path.c_str());
}

bool MicroSd::createFileIfNotExists(String path)
{
  if (!CardHealthy)
  {
    return false;
  }

  logger.AddEvent(PSTR("Create file/directory if not exists: ") + path);

  if (checkFile(path))
  {
    return true;
  }

  if (createDir(path))
  {
    return true;
  }

  logger.AddEvent(F("Failed to create file / directory."));
  return false;
}

bool MicroSd::appendFile(File *i_file, String *i_msg)
{
  xSemaphoreTake(sdCardMutex, portMAX_DELAY);
  bool status = false;

  if (!CardHealthy)
  {
    xSemaphoreGive(sdCardMutex);
    return false;
  }

  if (!*i_file)
  {
    logger.AddEvent("File not opened");
    return false;
  }
  else
  {
    if (i_file->print(i_msg->c_str()))
    {
      if (*i_file)
      {
        i_file->flush();

        if (!i_file->getWriteError())
        {
          status = true;
        }
        else
        {
          logger.AddEvent(F("Failed write to file"));
        }
      }
      else
      {
        logger.AddEvent(F("File not opened!"));
      }
    }
    else
    {
      logger.AddEvent(F("Failed write to file!"));
    }
  }

  xSemaphoreGive(sdCardMutex);
  return status;
}

uint16_t MicroSd::fileCount(String dirName, String fileNameFilter)
{
  if (!CardHealthy)
    return 0;

  File dir = SD_MMC.open(dirName.c_str());
  if (!dir || !dir.isDirectory())
    return 0;

  uint16_t fileCount = 0;
  File file = dir.openNextFile();
  while (file)
  {
    if (!file.isDirectory() && String(file.name()).indexOf(fileNameFilter) != -1)
    {
      fileCount++;
    }
    file = dir.openNextFile();
  }

  return fileCount;
}

int MicroSd::countFilesInDir(String path)
{
  return fileCount(path, "");
}

bool MicroSd::removeFilesInDir(String path, int maxFiles)
{

  File dir = SD_MMC.open(path.c_str());
  if (!dir || !dir.isDirectory())
  {
    return false;
  }

  int fileCount = 0;
  File file = dir.openNextFile();

  while (file && fileCount < maxFiles)
  {
    String fileName = path + "/" + file.name();
    if (SD_MMC.remove(fileName.c_str()))
    {
      fileCount++;
    }
    file = dir.openNextFile();
  }
  return fileCount > 0;
}

bool MicroSd::getCardHealthy()
{
  return CardHealthy;
}

uint16_t MicroSd::getCardSizeMB()
{
  return CardSizeMB;
}

uint16_t MicroSd::getCardTotalMB()
{
  return CardTotalMB;
}

uint16_t MicroSd::getCardUsedMB()
{
  return CardUsedMB;
}

uint16_t MicroSd::getCardFreeMB()
{
  return CardFreeMB;
}

uint8_t MicroSd::getFreeSpacePercent()
{
  return FreeSpacePercent;
}

uint8_t MicroSd::getUsedSpacePercent()
{
  return UsedSpacePercent;
}