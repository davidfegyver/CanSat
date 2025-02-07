
#include <Wire.h>
#include <Arduino.h>


#include <Adafruit_MPU6050.h>
#include <Adafruit_HMC5883_U.h>
#include <Adafruit_Sensor.h>

#include <MS5611.h>


#include "logging/logger.h"
#include "cfg.h"

class I2C {
public:
        I2C(Logger &logger, Logger &sensorLogger);
        
        void begin();
        void scan();
        void initMS5611();
        void initHMC5883L();
        void initMPU6050();
        

        void testMS5611();
        void testHMC5883L();
        void testMPU6050();


private:
        Logger &logger;
        Logger &sensorLogger;

        Adafruit_HMC5883_Unified mag;
        Adafruit_MPU6050 mpu;
        MS5611 barometer;


};