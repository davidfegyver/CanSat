#include "logger.h"
#include "micro_sd/micro_sd.h"

Logger::Logger(String i_FilePath, String i_FileName)
    : FileName(i_FileName), FilePath(i_FilePath)
{
  AddEvent(PSTR("Logger initialized with path: ") + FilePath + PSTR(", file: ") + FileName);
}

void Logger::openLogFile()
{
  sdCard->createFileIfNotExists(FilePath);

  AddEvent(PSTR("Opening log file: ") + FilePath + PSTR("/") + FileName);
  sdCard->openFile(&LogFile, FilePath + "/" + FileName);
}

void Logger::closeLogFile()
{
  AddEvent(PSTR("Closing log file: ") + FilePath + "/" + FileName);
  sdCard->closeFile(&LogFile);
}

bool Logger::checkIsLogFileHealthy()
{
  if (sdCard && sdCard->getCardHealthy())
  {
    return sdCard->isFileOpen(&LogFile);
  }
  return false;
}

void Logger::AddEvent(String msg, bool newLine, bool showTime)
{
  xSemaphoreTake(LogMutex, portMAX_DELAY);

  LastLogMsg = "";
  if (showTime)
  {
    LastLogMsg += getSystemTime() + " - ";
  }

  LastLogMsg += msg;

  if (newLine)
  {
    LastLogMsg += "\n";
  }

#if (ENABLE_SERIAL_LOGS == true)
  Serial.print(LastLogMsg);
#endif

  if (!checkIsLogFileHealthy())
  {
    FullLogMsg += LastLogMsg;
  }
  else
  {
    sdCard->appendFile(&LogFile, &LastLogMsg);
  }

  xSemaphoreGive(LogMutex);
}

bool Logger::getTimeSynced()
{
  return TimeSynced;
}

void Logger::setTimeSynced(bool i_data)
{
  TimeSynced = i_data;
  AddEvent(PSTR("System time synced: ") + getSystemTime());
}

String Logger::getFileName()
{
  return FileName;
}

String Logger::getFilePath()
{
  return FilePath;
}

void Logger::checkMaxLogFileSize()
{
  if (checkIsLogFileHealthy())
  {
    uint32_t FileSize = sdCard->getFileSize(FilePath + "/" + FileName);
    uint16_t file_count = sdCard->fileCount(FilePath, FileName);
    if (FileSize > MaxLogSize)
    {
      closeLogFile();
      sdCard->renameFile(FilePath + "/" + FileName, FilePath + "/" + FileName + String(file_count));
      openLogFile();
    }
  }
  else
  {
    if (FullLogMsg.length() > MaxLogSize)
    {
      FullLogMsg = "";
      AddEvent(F("In-memory log message cleared due to size limit."));
    }
  }
}

String Logger::getSystemTime()
{
  String ret = "0000-00-00_00-00-00";
  if (TimeSynced)
  {
    struct tm timeinfo;
    if (!getLocalTime(&timeinfo))
    {
      AddEvent(F("Failed to fetch local time."));
      return ret;
    }

    char timeString[20];
    strftime(timeString, sizeof(timeString), "%Y-%m-%d_%H-%M-%S", &timeinfo);
    ret = String(timeString);
  }
  return ret;
}

void Logger::connectSdCard(MicroSd *i_sdCard)
{
  sdCard = i_sdCard;
  if (sdCard && sdCard->getCardHealthy())
  {
    openLogFile();

    if (checkIsLogFileHealthy())
    {
      sdCard->appendFile(&LogFile, &FullLogMsg);
      FullLogMsg = "";
    }
  }

  checkMaxLogFileSize();
}