#include "lora.h"

Lora::Lora(Logger &logger, HardwareSerial &serialPort) : logger(logger), serialPort(serialPort) {
}

void Lora::begin() {
    serialPort.begin(LORA_BAUDRATE, SERIAL_8N1, LORA_RX, LORA_TX);
    delay(100);
    logger.addEvent(F("Lora module initialized"));

    radioSetFrequencyMHz(LORA_FREQ_MHZ);
    radioSetPower(LORA_POWER);
    radioSetPaBoost(LORA_PABOOST);
    radioSetCRC(LORA_CRC);
    delay(100);

    logger.addEvent(PSTR("Lora module version: ") + sysGetVersion());
    logger.addEvent(PSTR("Lora module frequency: ") + radioGetFrequency());
    logger.addEvent(PSTR("Lora module power: ") + radioGetPower());
    logger.addEvent(PSTR("Lora module modulation: ") + radioGetModulation());
    logger.addEvent(PSTR("Lora module spreading factor: ") + radioGetSF());
    logger.addEvent(PSTR("Lora module PA boost: ") + radioGetPaBoost());

}

String Lora::sendCommand(const String &command) {
    serialPort.println(command);
    delay(100);
    String response;
    while (serialPort.available()) {
        response += char(serialPort.read());
    }
    response.trim();
    return response;
}

// System Commands
String Lora::sysReset() {
    return sendCommand(F("sys reset"));
}

String Lora::sysSleep(const String &mode, uint32_t duration) {
    return sendCommand(PSTR("sys sleep ") + mode + " " + String(duration));
}

String Lora::sysFactoryReset() {
    return sendCommand(F("sys factoryRESET"));
}

String Lora::sysGetVersion() {
    return sendCommand(F("sys get ver"));
}

// Radio Commands
String Lora::radioSetFrequency(uint32_t frequency) {
    return sendCommand(PSTR("radio set freq ") + String(frequency));
}

String Lora::radioSetFrequencyMHz(float frequencyMHz) {
    uint32_t frequencyHz = frequencyMHz * 1e6;
    return radioSetFrequency(frequencyHz);
}

String Lora::radioSetPaBoost(const String &state) {
    return sendCommand("radio set pa " + state);
}

String Lora::radioSetPower(int power) {
    return sendCommand("radio set pwr " + String(power));
}

String Lora::radioSetModulation(const String &mode) {
    return sendCommand("radio set mod " + mode);
}

String Lora::radioSetCRC(const String &state) {
    return sendCommand("radio set crc " + state);
}

String Lora::radioGetCRC() {
    return sendCommand("radio get crc");
}

String Lora::radioGetModulation() {
    return sendCommand("radio get mod");
}

String Lora::radioGetFrequency() {
    return sendCommand("radio get freq");
}

float Lora::radioGetFrequencyMHz() {
    return radioGetFrequency().toFloat() / 1e6;
}

String Lora::radioGetPower() {
    return sendCommand("radio get pwr");
}

String Lora::radioGetPaBoost() {
    return sendCommand("radio get pa");
}

String Lora::radioSetSF(int spreadingFactor) {
    return sendCommand("radio set sf sf" + String(spreadingFactor));
}

String Lora::radioGetSF() {
    return sendCommand("radio get sf");
}

String Lora::radioTransmit(const String &data, int n) {
    String hexData;
    for (char c : data) {
        hexData += String(c, HEX);
    }
    return sendCommand("radio tx " + hexData + " " + String(n));
}
