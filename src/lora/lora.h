#pragma once

#include <Arduino.h>
#include "logging/logger.h"
#include "cfg.h"

class Lora
{
public:
    Lora(Logger &logger, HardwareSerial &serialPort);

    void begin();

    void sendCommand(const String &command);

    // System Commands
    void sysReset();
    void sysSleep(const String &mode, uint32_t duration);
    void sysFactoryReset();

    // Radio Commands
    void radioSetFrequency(uint32_t frequency);
    void radioSetFrequencyMHz(float frequencyMHz);
    void radioSetPaBoost(const String &state);
    void radioSetPower(int power);
    void radioSetModulation(const String &mode);
    void radioSetCRC(const String &state);
    void radioSetSF(int spreadingFactor);
    void radioSetCw(const String &state);
    void radioTransmit(const String &data, int n = 1);
    void radioSetBW(int bandwidth);

private:
    Logger &logger;
    HardwareSerial &serialPort;
};