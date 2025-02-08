#include "logger.h"
#include "globals.h"

Logger::Logger(String i_FilePath, String i_FileName)
    : FileName(i_FileName), FilePath(i_FilePath)
{
  addEvent(PSTR("Logger initialized with path: ") + FilePath + PSTR(", file: ") + FileName);
}

void Logger::openLogFile()
{
  sd_card.createFileIfNotExists(FilePath);
  addEvent(PSTR("Opening log file: ") + FilePath + PSTR("/") + FileName);
  sd_card.openFile(&LogFile, FilePath + "/" + FileName);
}

void Logger::closeLogFile()
{
  addEvent(PSTR("Closing log file: ") + FilePath + "/" + FileName);
  sd_card.closeFile(&LogFile);
}

bool Logger::checkIsLogFileHealthy()
{
  return sd_card.getCardHealthy() && sd_card.isFileOpen(&LogFile);
}

void Logger::addEvent(String msg, bool newLine, bool showTime)
{
  xSemaphoreTake(LogMutex, portMAX_DELAY);

  LastLogMsg = "";
  if (showTime)
  {
    LastLogMsg += (String(millis()) + " - ");
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
    sd_card.appendFile(&LogFile, &LastLogMsg);
  }

  xSemaphoreGive(LogMutex);

  if (!checkIsLogFileHealthy())
  {
    checkMaxLogFileSize();
  }
}

String Logger::getFileName() const
{
  return FileName;
}

String Logger::getFilePath() const
{
  return FilePath;
}

void Logger::checkMaxLogFileSize()
{
  if (checkIsLogFileHealthy())
  {
    uint32_t FileSize = sd_card.getFileSize(FilePath + "/" + FileName);
    uint16_t file_count = sd_card.fileCount(FilePath, FileName);

    if (FileSize > MaxLogSize)
    {
      closeLogFile();
      sd_card.renameFile(FilePath + "/" + FileName, FilePath + "/" + FileName + String(file_count));
      openLogFile();
    }
  }

  if (FullLogMsg.length() > MaxLogSize)
  {
    FullLogMsg = "";
    addEvent(F("In-memory log message cleared due to size limit."));
  }
}

void Logger::connectSdCard()
{
  if (sd_card.getCardHealthy())
  {
    addEvent(PSTR("SD card connected."));
    openLogFile();

    if (checkIsLogFileHealthy())
    {
      sd_card.appendFile(&LogFile, &FullLogMsg);
      FullLogMsg = "";
    }
  }

  checkMaxLogFileSize();
}

String Logger::getFullLogMsg()
{
  return FullLogMsg;
}