#include "i2c.h"

#include "globals.h"

I2C::I2C()
{
}

void I2C::begin()
{
    Wire.begin(I2C_SDA, I2C_SCL);
}

void I2C::scan()
{
    SystemLog.addEvent(F("Scanning for I2C devices ..."));

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

            SystemLog.addEvent(logString);
            nDevices++;
        }
    }
    if (nDevices == 0)
    {
        SystemLog.addEvent(F("No I2C devices found"));
    }
    else
    {
        SystemLog.addEvent(F("Scan complete"));
    }
}

void I2C::initMS5611()
{
    SystemLog.addEvent(F("Initializing HMC5883L (Pressure & Temperature) ..."));

    barometer.begin();

    Wire.beginTransmission((byte)MS5611_ADDRESS);
    int error = Wire.endTransmission();

    if (error != 0)
    {
        SystemLog.addEvent(F("MS5611 initialization failed"));
        barometerInitialized = false;
        return;
    }

    barometerInitialized = true;


    SystemLog.addEvent(F("MS5611 initialized"));
}

void I2C::initHMC5883L()
{
    SystemLog.addEvent(F("Initializing HMC5883L (Magnetometer) ..."));

    mag.begin();

    Wire.beginTransmission((byte)HMC5883_ADDRESS_MAG);
    int error = Wire.endTransmission();

    if (error != 0)
    {
        SystemLog.addEvent(F("HMC5883L initialization failed"));
        magInitialized = false;
        return;
    }

    magInitialized = true;
    SystemLog.addEvent(F("HMC5883L initialized"));
}

void I2C::initMPU6050()
{

    SystemLog.addEvent(F("Initializing MPU6050 (Gyro & Accelerometer - IMU) ..."));

    mpuInitialized = mpu.begin();

    if (!mpuInitialized)
    {
        SystemLog.addEvent(F("MPU6050 initialization failed"));
        return;
    }

    mpu.setGyroRange(GYRO_RANGE);
    mpu.setAccelerometerRange(ACCEL_RANGE);

    mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);

    mpu.setI2CBypass(true);
    mpu.enableSleep(false);

    SystemLog.addEvent(F("MPU6050 initialized"));
}

void I2C::createMPU6050Task()
{
    TaskHandle_t Task_MPU6050;
    SystemLog.addEvent(F("Creating MPU6050 Task"));

    xTaskCreatePinnedToCore(
        [](void *pvParameters)
        {
            I2C *i2c = static_cast<I2C *>(pvParameters);
            Adafruit_MPU6050 mpu = i2c->mpu;

            TickType_t xLastWakeTime = xTaskGetTickCount();

            while (true)
            {
                esp_task_wdt_reset();
                mpu.getEvent(&i2c->lastAccelerometerEvent, &i2c->lastGyroscopeEvent, &i2c->lastTemperatureEvent);

                String AlogString = "A:" + String(i2c->lastAccelerometerEvent.acceleration.x) + "," + String(i2c->lastAccelerometerEvent.acceleration.y) + "," + String(i2c->lastAccelerometerEvent.acceleration.z);
                String GlogString = "G:" + String(i2c->lastGyroscopeEvent.gyro.x) + "," + String(i2c->lastGyroscopeEvent.gyro.y) + "," + String(i2c->lastGyroscopeEvent.gyro.z);
                String TlogString = "t:" + String(i2c->lastTemperatureEvent.temperature);

                SensorLog.addEvent(AlogString);
                SensorLog.addEvent(GlogString);
                SensorLog.addEvent(TlogString);

                vTaskDelayUntil(&xLastWakeTime, TASK_MPU6050 / portTICK_PERIOD_MS);
            }
        },
        "MPU6050Task",
        2048,
        this,
        1,
        &Task_MPU6050,
        0);

    ESP_ERROR_CHECK(esp_task_wdt_add(Task_MPU6050));
}

void I2C::createHMC5883LTask()
{
    TaskHandle_t Task_HMC5883L;
    SystemLog.addEvent(F("Creating HMC5883L Task"));

    xTaskCreatePinnedToCore(
        [](void *pvParameters)
        {
            I2C *i2c = static_cast<I2C *>(pvParameters);
            Adafruit_HMC5883_Unified mag = i2c->mag;

            TickType_t xLastWakeTime = xTaskGetTickCount();

            while (true)
            {
                esp_task_wdt_reset();
                mag.getEvent(&i2c->lastMagnetometerEvent);

                String logString = "M:" + String(i2c->lastMagnetometerEvent.magnetic.x) + "," + String(i2c->lastMagnetometerEvent.magnetic.y) + "," + String(i2c->lastMagnetometerEvent.magnetic.z);
                SensorLog.addEvent(logString);

                vTaskDelayUntil(&xLastWakeTime, TASK_HMC5883L / portTICK_PERIOD_MS);
            }
        },
        "HMC5883LTask",
        2048,
        this,
        1,
        &Task_HMC5883L,
        0);

    ESP_ERROR_CHECK(esp_task_wdt_add(Task_HMC5883L));
}

void I2C::createMS5611Task()
{
    TaskHandle_t Task_MS5611;
    SystemLog.addEvent(F("Creating MS5611 Task"));

    xTaskCreatePinnedToCore(
        [](void *pvParameters)
        {
            I2C *i2c = static_cast<I2C *>(pvParameters);
            MS5611 barometer = i2c->barometer;

            TickType_t xLastWakeTime = xTaskGetTickCount();

            while (true)
            {
                esp_task_wdt_reset();

                i2c->lastTemperature = barometer.readTemperature();
                i2c->lastPressure = barometer.readPressure();

                String TlogString = "T:" + String(i2c->lastTemperature);
                String PlogString = "P:" + String(i2c->lastPressure);

                SensorLog.addEvent(TlogString);
                SensorLog.addEvent(PlogString);

                vTaskDelayUntil(&xLastWakeTime, TASK_MS5611 / portTICK_PERIOD_MS);
            }
        },
        "MS5611Task",
        2048,
        this,
        1,
        &Task_MS5611,
        0);

    ESP_ERROR_CHECK(esp_task_wdt_add(Task_MS5611));
}

sensors_event_t I2C::getLastAccelerometerEvent()
{
    return lastAccelerometerEvent;
}

sensors_event_t I2C::getLastGyroscopeEvent()
{
    return lastGyroscopeEvent;
}

sensors_event_t I2C::getLastMagnetometerEvent()
{
    return lastMagnetometerEvent;
}

float I2C::getLastTemperature()
{
    return lastTemperature;
}

float I2C::getLastPressure()
{
    return lastPressure;
}

bool I2C::isMPUInitialized()
{
    return mpuInitialized;
}

bool I2C::isMagInitialized()
{
    return magInitialized;
}

bool I2C::isBarometerInitialized()
{
    return barometerInitialized;
}


