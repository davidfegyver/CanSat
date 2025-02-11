#include "micro_sd.h"
#include "globals.h"

MicroSd::MicroSd() {}

void MicroSd::initCard()
{
  CardHealthy = true;
  SystemLog.addEvent(F("Initializing micro-SD Card"));

  if (!SD_MMC.begin("/sdcard", IS_SD_ONE_LINE))
  {
    handleCardError(F("SD Card Mount Failed"));
    return;
  }

  handleCardType(SD_MMC.cardType());
  checkCardHealth();
}

void MicroSd::handleCardError(const String &errorMessage)
{
  SystemLog.addEvent(errorMessage);
  CardHealthy = false;
}

void MicroSd::handleCardType(uint8_t cardType)
{
  const char *typeStr = nullptr;
  switch (cardType)
  {
  case CARD_MMC:
    typeStr = "MMC";
    break;
  case CARD_SD:
    typeStr = "SDSC";
    break;
  case CARD_SDHC:
    typeStr = "SDHC";
    break;
  default:
    handleCardError(F("No/unknown SD Card attached!"));
    return;
  }
  SystemLog.addEvent(PSTR("Card Type: ") + String(typeStr));
}

void MicroSd::checkCardHealth()
{
  if (!CardHealthy)
    return;

  updateCardUsageStatus();

  if (CardSizeMB == 0 || CardTotalMB == 0 || FreeSpacePercent <= 5)
  {
    handleCardError(F("SD Card health check failed: Low space or invalid size"));
  }
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

  SystemLog.addEvent(PSTR("Card usage - Size: ") + String(CardSizeMB) + F(" MB, Used: ") +
                     String(CardUsedMB) + F(" MB, Free: ") + String(CardFreeMB) + F(" MB, Free: ") +
                     String(FreeSpacePercent) + F("%"));
}

void MicroSd::reinitCard()
{
  SystemLog.addEvent(F("Reinitializing micro-SD Card"));
  SD_MMC.end();
  delay(50);
  initCard();
}

void MicroSd::cardCheckTask(void *pvParameters)
{
  SystemLog.addEvent(PSTR("Starting SD card health check task on core ") + String(xPortGetCoreID()));
  TickType_t xLastWakeTime = xTaskGetTickCount();

  while (true)
  {
    esp_task_wdt_reset();
    checkCardHealth();
    SystemLog.addEvent(PSTR("Card healthy status: ") + String(CardHealthy));

    if (!CardHealthy)
    {
      reinitCard();
    }

    esp_task_wdt_reset();
    vTaskDelayUntil(&xLastWakeTime, TASK_SDCARD / portTICK_PERIOD_MS);
  }
}

bool MicroSd::openFile(File &file, const String &path)
{
  if (!CardHealthy)
  {
    SystemLog.addEvent(F("Failed to open file - Card not healthy"));
    return false;
  }
  file = SD_MMC.open(path.c_str(), FILE_APPEND);
  if (!file)
  {
    handleCardError(PSTR("Failed to open file: ") + path);
    return false;
  }
  SystemLog.addEvent(PSTR("Opened file: ") + path);
  return true;
}

void MicroSd::closeFile(File &file)
{
  if (file)
  {
    file.close();
    SystemLog.addEvent(F("File closed successfully"));
  }
  else
  {
    SystemLog.addEvent(F("Attempted to close an unopened file"));
  }
}

bool MicroSd::renameFile(const String &path1, const String &path2)
{
  if (!CardHealthy)
  {
    SystemLog.addEvent(F("Failed to rename file - Card not healthy"));
    return false;
  }

  SystemLog.addEvent(PSTR("Renaming file: ") + path1 + PSTR(" to ") + path2);
  return SD_MMC.rename(path1.c_str(), path2.c_str());
}

uint32_t MicroSd::getFileSize(const String &path)
{
  if (!CardHealthy)
  {
    SystemLog.addEvent(F("Failed to get file size - Card not healthy"));
    return 0;
  }

  File file = SD_MMC.open(path.c_str(), FILE_READ);
  if (!file)
  {
    handleCardError(PSTR("Failed to open file to get size: ") + path);
    return 0;
  }
  uint32_t size = file.size();
  file.close();

  SystemLog.addEvent(PSTR("File size for ") + path + PSTR(": ") + String(size) + PSTR(" bytes"));
  return size;
}

bool MicroSd::createDir(const String &path)
{
  if (!CardHealthy)
  {
    SystemLog.addEvent(F("Failed to create directory - Card not healthy"));
    return false;
  }

  SystemLog.addEvent(PSTR("Creating directory: ") + path);
  return SD_MMC.mkdir(path.c_str());
}

bool MicroSd::removeDir(const String &path)
{
  if (!CardHealthy)
  {
    SystemLog.addEvent(F("Failed to remove directory - Card not healthy"));
    return false;
  }

  SystemLog.addEvent(PSTR("Removing directory: ") + path);
  return SD_MMC.rmdir(path.c_str());
}

bool MicroSd::checkFile(const String &path)
{
  if (!CardHealthy)
  {
    SystemLog.addEvent(F("Failed to check file/directory - Card not healthy"));
    return false;
  }

  SystemLog.addEvent(PSTR("Checking existence of file/directory: ") + path);
  return SD_MMC.exists(path.c_str());
}

bool MicroSd::createFile(const String &path)
{
  if (!CardHealthy)
  {
    SystemLog.addEvent(F("Failed to create file - Card not healthy"));
    return false;
  }
  SystemLog.addEvent(PSTR("Creating file: ") + path);
  File file = SD_MMC.open(path.c_str(), FILE_WRITE);
  if (!file)
  {
    SystemLog.addEvent(PSTR("Failed to create file: ") + path);
    return false;
  }
  file.close();
  return true;
}

bool MicroSd::removeFile(const String &path)
{
  if (!CardHealthy)
  {
    SystemLog.addEvent(F("Failed to remove file - Card not healthy"));
    return false;
  }
  SystemLog.addEvent(PSTR("Removing file: ") + path);
  return SD_MMC.remove(path.c_str());
}

bool MicroSd::appendFile(File &file, const String &message)
{
  xSemaphoreTake(sdCardMutex, portMAX_DELAY);
  if (!CardHealthy || !file)
  {
    SystemLog.addEvent(F("Failed to append to file - Card not healthy or file not open"));
    xSemaphoreGive(sdCardMutex);
    return false;
  }
  bool status = file.print(message.c_str()) && !file.getWriteError();
  if (status)
    file.flush();
  else
    SystemLog.addEvent(F("Failed to write to file"));
  xSemaphoreGive(sdCardMutex);
  return status;
}

void MicroSd::sendFileToClient(AsyncWebServerRequest *request, const String &path)
{
  xSemaphoreTake(sdCardMutex, portMAX_DELAY);
  if (!CardHealthy)
  {
    xSemaphoreGive(sdCardMutex);
    request->send(500, "text/plain", "SD card not healthy");
    return;
  }

  File file = SD_MMC.open(path, FILE_READ);
  if (!file)
  {
    xSemaphoreGive(sdCardMutex);
    request->send(500, "text/plain", "Failed to open file");
    return;
  }

  request->send(file, path, "application/octet-stream");
  file.close();
  xSemaphoreGive(sdCardMutex);
}

void MicroSd::formatCard()
{
  if (!CardHealthy)
    return;
  SystemLog.addEvent(F("Formatting SD card..."));

  char drv[3] = {'0', ':', 0};
  void *workbuf = ff_memalloc(4096);
  if (!workbuf)
  {
    SystemLog.addEvent(F("Failed to allocate memory for workbuf"));
    return;
  }

  FRESULT res = f_mkfs(drv, FM_ANY, 16 * 1024, workbuf, 4096);
  free(workbuf);
  SystemLog.addEvent(res == FR_OK ? F("Format complete") : F("Format failed"));
}

bool MicroSd::WritePicture(const String &photoName, const uint8_t *photoData, size_t photoLen)
{
  if (xSemaphoreTake(sdCardMutex, portMAX_DELAY) != pdTRUE)
  {
    SystemLog.addEvent(F("Failed to acquire semaphore"));
    return false;
  }

  if (!CardHealthy)
  {
    xSemaphoreGive(sdCardMutex);
    SystemLog.addEvent(F("Failed to write picture - Card not healthy"));
    return false;
  }

  File file = SD_MMC.open(photoName, FILE_WRITE);
  if (!file)
  {
    xSemaphoreGive(sdCardMutex);
    SystemLog.addEvent(F("Failed to write picture - Could not open file"));
    return false;
  }

  size_t bytesWritten = file.write(photoData, photoLen);

  file.flush();
  file.close();
  xSemaphoreGive(sdCardMutex);

  return bytesWritten == photoLen;
}

uint16_t MicroSd::fileCount(const String &dirName, const String &fileNameFilter)
{
  if (xSemaphoreTake(sdCardMutex, portMAX_DELAY) != pdTRUE)
  {
    SystemLog.addEvent(F("Failed to acquire semaphore"));
    return 0;
  }

  if (!CardHealthy)
  {
    xSemaphoreGive(sdCardMutex);
    SystemLog.addEvent(F("Failed to count files - Card not healthy"));
    return 0;
  }

  File dir = SD_MMC.open(dirName.c_str());
  if (!dir || !dir.isDirectory())
  {
    xSemaphoreGive(sdCardMutex);
    SystemLog.addEvent(PSTR("Invalid directory: ") + dirName);
    return 0;
  }

  uint16_t fileCount = 0;
  File file = dir.openNextFile();

  while (file)
  {
    if (String(file.name()).indexOf(fileNameFilter) != -1)
    {
      fileCount++;
    }
    file = dir.openNextFile();
  }

  xSemaphoreGive(sdCardMutex);

  SystemLog.addEvent(PSTR("Files counted in directory ") + dirName + PSTR(": ") + String(fileCount));
  return fileCount;
}

bool MicroSd::isFileOpen(File &file)
{
  return file;
}



bool MicroSd::getCardHealthy() { return CardHealthy; }
uint16_t MicroSd::getCardSizeMB() { return CardSizeMB; }
uint16_t MicroSd::getCardTotalMB() { return CardTotalMB; }
uint16_t MicroSd::getCardUsedMB() { return CardUsedMB; }
uint16_t MicroSd::getCardFreeMB() { return CardFreeMB; }
uint8_t MicroSd::getFreeSpacePercent() { return FreeSpacePercent; }
uint8_t MicroSd::getUsedSpacePercent() { return UsedSpacePercent; }
