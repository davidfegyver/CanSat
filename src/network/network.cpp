
#include "network.h"

Network::Network(Logger &i_logger) : logger(i_logger) {}

void Network::init()
{
    logger.addEvent(F("Initializing WiFi"));
    WiFi.mode(WIFI_MODE_AP);
    WiFi.softAPConfig(IPAddress(WIFI_IP), IPAddress(WIFI_IP), IPAddress(WIFI_SUBNET));
    WiFi.softAP(WIFI_SSID, WIFI_PASSWORD, WIFI_CHANNEL, 0, WIFI_MAX_CLIENTS);
    logger.addEvent(PSTR("Connect at: http://") + WiFi.softAPIP().toString());

    logger.addEvent(PSTR("WiFi MAC: ") + WiFi.macAddress());
    WiFi.setHostname(DEVICE_HOSTNAME);

    logger.addEvent(F("WiFi initialized successfully"));

    
}
