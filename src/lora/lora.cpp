#include "lora.h"

#include "globals.h"

Lora::Lora(HardwareSerial &serialPort) : serialPort(serialPort)
{
}

void Lora::begin()
{
    serialPort.begin(LORA_BAUD, SERIAL_8N1, -1, LORA_TX);

    SystemLog.addEvent(F("Lora module initialized"));

    radioSetFrequencyMHz(LORA_FREQ_MHZ);
    radioSetPower(LORA_POWER);
    radioSetPaBoost(LORA_PABOOST);
    radioSetCRC(LORA_CRC);
    radioSetBW(LORA_BANDWIDTH);
}

void Lora::sendCommand(const String &command)
{
    serialPort.println(command);
    delay(100);
}

// System Commands
void Lora::sysReset()
{
    sendCommand(F("sys reset"));
}

void Lora::sysSleep(const String &mode, uint32_t duration)
{
    sendCommand(PSTR("sys sleep ") + mode + " " + String(duration));
}

void Lora::sysFactoryReset()
{
    sendCommand(F("sys factoryRESET"));
}

// Radio Commands
void Lora::radioSetFrequency(uint32_t frequency)
{
    sendCommand(PSTR("radio set freq ") + String(frequency));
}

void Lora::radioSetFrequencyMHz(float frequencyMHz)
{
    uint32_t frequencyHz = frequencyMHz * 1e6;
    radioSetFrequency(frequencyHz);
}

void Lora::radioSetPaBoost(const String &state)
{
    sendCommand("radio set pa " + state);
}

void Lora::radioSetPower(int power)
{
    sendCommand("radio set pwr " + String(power));
}

void Lora::radioSetModulation(const String &mode)
{
    sendCommand("radio set mod " + mode);
}

void Lora::radioSetCRC(const String &state)
{
    sendCommand("radio set crc " + state);
}

void Lora::radioSetSF(int spreadingFactor)
{
    sendCommand("radio set sf sf" + String(spreadingFactor));
}

void Lora::radioSetBW(int bandwidth)
{
    sendCommand("radio set bw " + String(bandwidth));
}

void Lora::radioSetCw(const String &state)
{
    sendCommand("radio cw " + state);
}

void Lora::radioTransmit(const String &data, int n)
{
    String hexData;
    for (char c : data)
    {
        hexData += String(c, HEX);
    }
    sendCommand("radio tx " + hexData + " " + String(n));
}
