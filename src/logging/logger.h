#pragma once

#include <Arduino.h>
#include <FS.h>

class MicroSd;

class Logger
{
private:
  MicroSd *sdCard = nullptr;
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

  void connectSdCard(MicroSd *sdCard);
  void openLogFile();
  void closeLogFile();
  bool checkIsLogFileHealthy();
  void addEvent(String message, bool newLine = true, bool showTime = true);

  String getFileName() const;
  String getFilePath() const;
  bool getTimeSynced() const;
  void setTimeSynced(bool isSynced);

  String getSystemTime();
};
