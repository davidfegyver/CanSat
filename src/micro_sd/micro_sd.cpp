#include "micro_sd.h"
#include "logging/logger.h"

MicroSd::MicroSd(Logger &i_logger) : logger(i_logger) {}

void MicroSd::initCard()
{
  CardHealthy = true;
  CardHealthyOnBoot = true;

  logger.addEvent(F("Initializing micro-SD Card"));

  if (!SD_MMC.begin("/sdcard", IS_SD_ONE_LINE))
  {
    logger.addEvent(F("SD Card Mount Failed"));
    CardHealthyOnBoot = false;
    CardHealthy = false;
    return;
  }

  CardType = SD_MMC.cardType();

  switch (CardType)
  {
  case CARD_MMC:
    logger.addEvent(F("Card Type: MMC"));
    break;
  case CARD_SD:
    logger.addEvent(F("Card Type: SDSC"));
    break;
  case CARD_SDHC:
    logger.addEvent(F("Card Type: SDHC"));
    break;
  default:
    logger.addEvent(F("No/unknown SD Card attached!"));
    CardHealthyOnBoot = false;
    CardHealthy = false;
    return;
  }

  checkCardHealth();

  if (CardHealthy)
  {
    logger.addEvent(F("SD Card initialized successfully"));
  }
  else
  {
    CardHealthyOnBoot = false;
    logger.addEvent(F("SD Card initialization failed!"));
  }
}

void MicroSd::reinitCard()
{
  logger.addEvent(F("Reinitializing micro-SD Card"));
  SD_MMC.end();
  delay(50);
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

  logger.addEvent(PSTR("Card usage - Size: ") + String(CardSizeMB) + F(" MB, Used: ") + String(CardUsedMB) + F(" MB, Free: ") + String(CardFreeMB) + F(" MB, Free: ") + String(FreeSpacePercent) + F("%"));
}

void MicroSd::checkCardHealth()
{
  if (!CardHealthy)
    return;

  // meow, check if card is still in slot

  updateCardUsageStatus();

  if (CardSizeMB == 0 || CardTotalMB == 0 || FreeSpacePercent <= 5)
  {
    logger.addEvent(F("SD Card health check failed: Low space or invalid size"));
    CardHealthy = false;
  }
}

void MicroSd::cardCheckTask(void *pvParameters)
{
  logger.addEvent(PSTR("Starting SD card health check task on core ") + String(xPortGetCoreID()));
  TickType_t xLastWakeTime = xTaskGetTickCount();

  while (true)
  {
    esp_task_wdt_reset();
    checkCardHealth();

    logger.addEvent(PSTR("Card healthy status: ") + String(CardHealthy));

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
    logger.addEvent(F("Failed to open file - Card not healthy"));
    return false;
  }

  *i_file = SD_MMC.open(i_path.c_str(), FILE_APPEND);
  if (!*i_file)
  {
    logger.addEvent(PSTR("Failed to open file: ") + i_path);
    CardHealthy = false;
    return false;
  }

  logger.addEvent(PSTR("Opened file: ") + i_path);
  return true;
}

void MicroSd::closeFile(File *i_file)
{
  if (*i_file)
  {
    i_file->close();
    logger.addEvent(F("File closed successfully"));
  }
  else
  {
    logger.addEvent(F("Attempted to close an unopened file"));
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
    logger.addEvent(F("Failed to rename file - Card not healthy"));
    return false;
  }

  logger.addEvent(PSTR("Renaming file: ") + path1 + PSTR(" to ") + path2);
  return SD_MMC.rename(path1.c_str(), path2.c_str());
}

bool MicroSd::deleteFile(String path)
{
  if (!CardHealthy)
  {
    logger.addEvent(F("Failed to delete file - Card not healthy"));
    return false;
  }

  logger.addEvent(PSTR("Deleting file: ") + path);
  return SD_MMC.remove(path.c_str());
}

uint32_t MicroSd::getFileSize(String path)
{
  if (!CardHealthy)
  {
    logger.addEvent(F("Failed to get file size - Card not healthy"));
    return 0;
  }

  File file = SD_MMC.open(path.c_str(), FILE_APPEND);
  if (!file)
  {
    logger.addEvent(PSTR("Failed to open file to get size: ") + path);
    return 0;
  }

  uint32_t size = file.size();
  file.close();

  logger.addEvent(PSTR("File size for ") + path + PSTR(": ") + String(size) + PSTR(" bytes"));
  return size / 1024;
}

bool MicroSd::createDir(String path)
{
  if (!CardHealthy)
  {
    logger.addEvent(F("Failed to create directory - Card not healthy"));
    return false;
  }

  logger.addEvent(PSTR("Creating directory: ") + path);
  return SD_MMC.mkdir(path.c_str());
}

bool MicroSd::removeDir(String path)
{
  if (!CardHealthy)
  {
    logger.addEvent(F("Failed to remove directory - Card not healthy"));
    return false;
  }

  logger.addEvent(PSTR("Removing directory: ") + path);
  return SD_MMC.rmdir(path.c_str());
}

bool MicroSd::checkFile(String path)
{
  if (!CardHealthy)
  {
    logger.addEvent(F("Failed to check file/directory - Card not healthy"));
    return false;
  }

  logger.addEvent(PSTR("Checking existence of file/directory: ") + path);
  return SD_MMC.exists(path.c_str());
}

bool MicroSd::createFile(String path)
{
  if (!CardHealthy)
  {
    logger.addEvent(F("Failed to create file - Card not healthy"));
    return false;
  }

  logger.addEvent(PSTR("Creating file: ") + path);
  File file = SD_MMC.open(path.c_str(), FILE_WRITE);
  if (file)
  {
    file.close();
    return true;
  }
  logger.addEvent(PSTR("Failed to create file: ") + path);
  return false;
}

bool MicroSd::removeFile(String path)
{
  if (!CardHealthy)
  {
    logger.addEvent(F("Failed to remove file - Card not healthy"));
    return false;
  }

  logger.addEvent(PSTR("Removing file: ") + path);
  return SD_MMC.remove(path.c_str());
}

bool MicroSd::createFileIfNotExists(String path)
{
  if (!CardHealthy)
  {
    logger.addEvent(F("Failed to create file/directory if not exists - Card not healthy"));
    return false;
  }

  logger.addEvent(PSTR("Creating file/directory if not exists: ") + path);

  if (checkFile(path))
  {
    return true;
  }

  if (createDir(path))
  {
    return true;
  }

  logger.addEvent(PSTR("Failed to create file/directory: ") + path);
  return false;
}

bool MicroSd::appendFile(File *i_file, String *i_msg)
{
  xSemaphoreTake(sdCardMutex, portMAX_DELAY);
  bool status = false;

  if (!CardHealthy)
  {
    logger.addEvent(F("Failed to append to file - Card not healthy"));
    xSemaphoreGive(sdCardMutex);
    return false;
  }

  if (!*i_file)
  {
    logger.addEvent("File not opened");
    xSemaphoreGive(sdCardMutex);
    return false;
  }

  if (i_file->print(i_msg->c_str()))
  {
    i_file->flush();

    if (!i_file->getWriteError())
    {
      status = true;
    }
    else
    {
      logger.addEvent(F("Failed write to file - Write error"));
    }
  }
  else
  {
    logger.addEvent(F("Failed write to file"));
  }

  xSemaphoreGive(sdCardMutex);
  return status;
}

uint16_t MicroSd::fileCount(String dirName, String fileNameFilter)
{
  if (!CardHealthy)
  {
    logger.addEvent(F("Failed to count files - Card not healthy"));
    return 0;
  }

  File dir = SD_MMC.open(dirName.c_str());
  if (!dir || !dir.isDirectory())
  {
    logger.addEvent(PSTR("Invalid directory: ") + dirName);
    return 0;
  }

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

  logger.addEvent(PSTR("Files counted in directory ") + dirName + PSTR(": ") + String(fileCount));
  return fileCount;
}

int MicroSd::countFilesInDir(String path)
{
  return fileCount(path, "");
}

bool MicroSd::removeFilesInDir(String path, int maxFiles)
{
  if (!CardHealthy)
  {
    logger.addEvent(F("Failed to remove files - Card not healthy"));
    return false;
  }

  File dir = SD_MMC.open(path.c_str());
  if (!dir || !dir.isDirectory())
  {
    logger.addEvent(PSTR("Invalid directory for file removal: ") + path);
    return false;
  }

  int fileCount = 0;
  File file = dir.openNextFile();
  while (file && fileCount < maxFiles)
  {
    String fileName = path + "/" + file.name();
    if (SD_MMC.remove(fileName.c_str()))
    {
      logger.addEvent(PSTR("Removed file: ") + fileName);
      fileCount++;
    }
    else
    {
      logger.addEvent(PSTR("Failed to remove file: ") + fileName);
    }
    file = dir.openNextFile();
  }

  logger.addEvent(PSTR("Total files removed from ") + path + F(": ") + String(fileCount));
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

bool MicroSd::WritePicture(const String &photoName, const uint8_t *photoData, size_t photoLen) {
    if (!CardHealthy) {
        logger.addEvent(F("Failed to write picture - Card not healthy"));
        return false;
    }

    bool status = false;

    File file = SD_MMC.open(photoName, FILE_WRITE);

    if (!file) {
        logger.addEvent(PSTR("Failed to write picture - Could not open file: ") + photoName);
        return status;
    }

    size_t bytesWritten = file.write(photoData, photoLen);

    if (bytesWritten != photoLen) {
        logger.addEvent(F("Failed to write picture - Error while writing to file"));
    } else {
        logger.addEvent(PSTR("Picture written successfully: ") + photoName);
        status = true;
    }

    file.close();

    return status;
}

void MicroSd::sendFileToClient(AsyncWebServerRequest *request, const String &path) {
    if (!CardHealthy) {
        logger.addEvent(F("Failed to send file to client - Card not healthy"));
        request->send(500, "text/plain", "SD card not healthy");
        return;
    }

    File file = SD_MMC.open(path, FILE_READ);

    if (!file) {
        logger.addEvent(PSTR("Failed to send file to client - Could not open file: ") + path);
        request->send(500, "text/plain", "Failed to open file");
        return;
    }

    AsyncWebServerResponse *response = request->beginChunkedResponse("application/octet-stream", [file](uint8_t *buffer, size_t maxLen, size_t index) mutable -> size_t {
        size_t chunkSize = file.read(buffer, maxLen);
        return chunkSize;
    });

    response->addHeader("Content-Disposition", "attachment; filename=" + path);
    request->send(response);
}