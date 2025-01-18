#pragma once

#include <Arduino.h>
#include <FS.h>
#include <WiFi.h>
#include "cfg.h"
#include "logging/logger.h"

class Network
{
private:
  Logger &logger;

public:
  Network(Logger &i_logger);
  


  void init();
};
