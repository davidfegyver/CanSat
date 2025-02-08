#pragma once

#include <Wire.h>
#include <Arduino.h>

#include <Adafruit_MPU6050.h>
#include <Adafruit_HMC5883_U.h>
#include <Adafruit_Sensor.h>

#include <MS5611.h>

#include "logging/logger.h"
#include "cfg.h"
#include "esp_task_wdt.h"
#include "esp32-hal-cpu.h"

class I2C
{
public:
        I2C();

        void begin();
        void scan();
        void initMS5611();
        void initHMC5883L();
        void initMPU6050();

        void createMPU6050Task();
        void createHMC5883LTask();
        void createMS5611Task();

        sensors_event_t getLastAccelerometerEvent();
        sensors_event_t getLastGyroscopeEvent();
        sensors_event_t getLastMagnetometerEvent();
        float getLastTemperature();
        float getLastRelativePressure();
        float getLastRelativeAltitude();

private:
        Adafruit_HMC5883_Unified mag;
        Adafruit_MPU6050 mpu;
        MS5611 barometer;

        sensors_event_t lastAccelerometerEvent;
        sensors_event_t lastGyroscopeEvent;
        sensors_event_t lastTemperatureEvent;
        sensors_event_t lastMagnetometerEvent;

        float lastTemperature;
        float lastRealPressure;
};