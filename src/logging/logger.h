#pragma once

#include <Arduino.h>
#include <FS.h>

class Logger
{
private:
  String FilePath;
  String FileName;
  uint16_t MaxLogSize = 1024;
  bool TimeSynced = false;
  String LastLogMsg;
  String FullLogMsg;
  File LogFile;
  SemaphoreHandle_t LogMutex = xSemaphoreCreateMutex();

  void checkMaxLogFileSize();

public:
  Logger(String filePath, String fileName);
  ~Logger() = default;

  void connectSdCard();
  void openLogFile();
  void closeLogFile();
  bool checkIsLogFileHealthy();
  void addEvent(String message, bool newLine = true, bool showTime = true);

  String getFileName() const;
  String getFilePath() const;

  String getFullLogMsg();
};
