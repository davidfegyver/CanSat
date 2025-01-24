#pragma once

#include <Arduino.h>
#include "logging/logger.h"
#include "cfg.h"

class Lora
{
public:
    Lora(Logger &logger, HardwareSerial &serialPort);

    void begin();

    String sendCommand(const String &command);

    // System Commands
    String sysReset();
    String sysSleep(const String &mode, uint32_t duration);
    String sysFactoryReset();
    String sysGetVersion();

    // Radio Commands
    String radioSetFrequency(uint32_t frequency);
    String radioSetFrequencyMHz(float frequencyMHz);
    String radioSetPaBoost(const String &state);
    String radioSetPower(int power);
    String radioSetModulation(const String &mode);
    String radioSetCRC(const String &state);
    String radioGetCRC();
    String radioGetModulation();
    String radioGetFrequency();
    float radioGetFrequencyMHz();
    String radioGetPower();
    String radioGetPaBoost();
    String radioSetSF(int spreadingFactor);
    String radioGetSF();
    String radioTransmit(const String &data, int n = 1);

private:
    Logger &logger;
    HardwareSerial &serialPort;
};