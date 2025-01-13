#pragma once

#include <Arduino.h>
#include <FS.h>

class MicroSd;

class Logger
{
private:
  MicroSd *sdCard;
  String FileName = "";
  String FilePath = "";
  uint16_t MaxLogSize = 1024;
  bool TimeSynced = false;
  String LastLogMsg = "";
  String FullLogMsg = "";
  File LogFile;
  SemaphoreHandle_t LogMutex = xSemaphoreCreateMutex();

public:
  Logger(String, String);
  ~Logger() {};

  void openLogFile();
  void closeLogFile();
  bool checkIsLogFileHealthy();
  void AddEvent(String, bool = true, bool = true);
  void setTimeSynced(bool);

  String getFileName();
  String getFilePath();
  bool getTimeSynced();
  void checkMaxLogFileSize();

  String getSystemTime();

  void connectSdCard(MicroSd *);
};
