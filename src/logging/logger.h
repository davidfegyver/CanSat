#pragma once

#include <Arduino.h>
#include <FS.h>

class MicroSd;

class Logger
{
private:
  MicroSd *sdCard = nullptr;
  String FileName;
  String FilePath;
  uint16_t MaxLogSize = 1024;
  bool TimeSynced = false;
  String LastLogMsg;
  String FullLogMsg;
  File LogFile;
  SemaphoreHandle_t LogMutex = xSemaphoreCreateMutex();

public:
  Logger(String filePath, String fileName);
  ~Logger() = default;

  void openLogFile();
  void closeLogFile();
  bool checkIsLogFileHealthy();
  void AddEvent(String message, bool newLine = true, bool showTime = true);
  void setTimeSynced(bool isSynced);

  String getFileName();
  String getFilePath();
  bool getTimeSynced();
  void checkMaxLogFileSize();

  String getSystemTime();

  void connectSdCard(MicroSd *sdCard);
};
