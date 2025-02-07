#include "i2c.h"

I2C::I2C(Logger &logger, Logger &sensorLogger) : logger(logger), sensorLogger(sensorLogger)
{
}

void I2C::begin()
{
    Wire.begin(I2C_SDA, I2C_SCL);
}

void I2C::scan()
{
    logger.addEvent(F("Scanning for I2C devices ..."));

    byte error, address;
    int nDevices;

    nDevices = 0;
    for (address = 0x8; address < 0x7f; address++)
    {
        Wire.beginTransmission(address);
        error = Wire.endTransmission();

        if (error == 0)
        {
            String logString = F("I2C device found at address 0x");
            if (address < 16)
            {
                logString += F("0");
            }
            logString += String(address, HEX);

            logger.addEvent(logString);
            nDevices++;
        }
    }
    if (nDevices == 0)
    {
        logger.addEvent(F("No I2C devices found"));
    }
    else
    {
        logger.addEvent(F("Scan complete"));
    }
}

void I2C::initMS5611()
{
    logger.addEvent(F("Initializing HMC5883L (Pressure & Temperature) ..."));

    barometer.begin();

    logger.addEvent(F("MS5611 initialized"));
}

void I2C::testMS5611()
{

    double myRealAltitude = 335;

    double realTemperature = barometer.readTemperature();
    double realPressure = barometer.readPressure();

    double seaLevelPressure = barometer.getSeaLevel(realPressure, myRealAltitude);

    logger.addEvent("realTemp = " + String(realTemperature) + " *C");

    logger.addEvent("realPressure = " + String(realPressure / 100) + " hPa");

    logger.addEvent("seaLevelPressure = " + String(seaLevelPressure / 100) + " hPa");
}

void I2C::initHMC5883L()
{
    logger.addEvent(F("Initializing HMC5883L (Magnetometer) ..."));

    mag.begin();

    logger.addEvent(F("HMC5883L initialized"));
}

void I2C::testHMC5883L()
{
    sensors_event_t event;
    mag.getEvent(&event);

    logger.addEvent(String(event.magnetic.x) + ",", false, false);
    logger.addEvent(String(event.magnetic.y) + ",", false, false);
    logger.addEvent(String(event.magnetic.z) + ",", true, false);

    float heading = atan2(event.magnetic.y, event.magnetic.x);

    // http://www.magnetic-declination.com/
    float declinationAngle = 0.09;
    heading += declinationAngle;

    if (heading < 0)
        heading += 2 * PI;

    if (heading > 2 * PI)
        heading -= 2 * PI;

    float headingDegrees = heading * 180 / M_PI;

    logger.addEvent("Heading: " + String(headingDegrees) + " degrees");
}

void I2C::initMPU6050()
{

    logger.addEvent(F("Initializing MPU6050 (Gyro & Accelerometer - IMU) ..."));
    while (!mpu.begin())
    {
        Serial.println("Could not find a valid MPU6050 sensor, check wiring!");
        delay(500);
    }

    logger.addEvent(F("MPU6050 2"));

    mpu.setGyroRange(GYRO_RANGE);
    mpu.setAccelerometerRange(ACCEL_RANGE);

    mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);

    mpu.setI2CBypass(true);
    mpu.enableSleep(false);

    logger.addEvent(F("MPU6050 initialized"));
}

void I2C::testMPU6050()
{

    sensors_event_t a, g, temp;
    mpu.getEvent(&a, &g, &temp);
    logger.addEvent("AccelX: " + String(a.acceleration.x) + "   ", false, false);
    logger.addEvent("AccelY: " + String(a.acceleration.y) + "   ", false, false);
    logger.addEvent("AccelZ: " + String(a.acceleration.z) + "   ", false);
    logger.addEvent("GyroX: " + String(g.gyro.x));
    logger.addEvent("GyroY: " + String(g.gyro.y));
    logger.addEvent("GyroZ: " + String(g.gyro.z));
}