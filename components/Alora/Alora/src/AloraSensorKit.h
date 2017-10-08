/*
 * Originally written by Andri Yadi on 8/5/16
 * Maintained by Alwin Arrasyid
 */

#ifndef ALORA_ALORASENSORKIT_H
#define ALORA_ALORASENSORKIT_H

#include <Wire.h>
#include <Arduino.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <ClosedCube_HDC1080.h>
#include <NMEAGPS.h>
#include <Streamers.h>
#include "Adafruit_TSL2591.h"
#include <SparkFunCCS811.h>
#include "SparkFunLSM9DS1.h"
#include "GpioExpander.h"

#include <RTClib.h>
#undef SECONDS_PER_DAY

#include "AllAboutEE_MAX11609.h"
using namespace AllAboutEE;

#ifndef ALORA_SENSOR_USE_CCS811
#define ALORA_SENSOR_USE_CCS811 1
#endif

#ifndef ALORA_SENSOR_QUERY_INTERVAL
#define ALORA_SENSOR_QUERY_INTERVAL 300
#endif

#ifndef ALORA_ENABLE_PIN
#define ALORA_ENABLE_PIN 16
#endif

#define ALORA_HDC1080_ADDRESS 0x40
#define ALORA_I2C_ADDRESS_CCS811 0x5A
#define ALORA_I2C_ADDRESS_IMU_M 0x1E
#define ALORA_I2C_ADDRESS_IMU_AG 0x6B

#define ALORA_MAGNETIC_SENSOR_PIN 35

#define ALORA_ADC_GAS_HEATER_PIN 13
#define ALORA_ADC_GAS_CHANNEL 1

#define ALORA_WINDSENSOR_PIN 34
static uint32_t aloraWindTimeSinceLastTick = 0;
static uint32_t aloraWindLastTick = 0;

//static void aloraWindSensorInterruptHandler();

/**
 * Data read from sensors are stored in this struct
 */
struct SensorValues {
    float T1;           /**< Temperature from BME280 in celcius unit */
    float P;            /**< Pressure from BME280 in hPa unit */
    float H1;           /**< Humidity from BME280 */
    float T2;           /**< Temperature from HDC1080 in celcius unit */
    float H2;           /**< Humidifty from HDC1080 */
    double lux;         /**< Luminance from TSL2591 */
    uint16_t gas;       /**< Air quality value from either CCS811 or analog gas sensor */
    uint16_t co2;       /**< CO2 readgin from CCS811 */
    float accelX;       /**< Accelerometer X axis */
    float accelY;       /**< Accelerometer Y axis */
    float accelZ;       /**< Accelerometer Z axis */
    float gyroX;        /**< Gyroscope X axis */
    float gyroY;        /**< Gyroscope Y axis */
    float gyroZ;        /**< Gyroscope Z axis */
    float magX;         /**< Magnometer X axis */
    float magY;         /**< Magnometer Y axis */
    float magZ;         /**< Magnometer Z axis */
    float magHeading;   /**< Heading in degrees */
    int magnetic;       /**< Magnetic sensor value */
    float windSpeed;    /**< Speed of the wind in MPH */
    gps_fix gpsFix;     /**< GPS fix information */
};


/**
 *  Alora Sensor Kit class.
 *  Main class for reading sensor on Alora board
 *  \example examples/AloraReadAllSensor/AloraReadAllSensor.ino
 *  \example examples/AloraReadGPS/AloraReadGPS.ino
 */
class AloraSensorKit {
public:
    AloraSensorKit();
    ~AloraSensorKit();

    void begin();
    void run();
    void scanAndPrintI2C(Print& print);
    void printSensingTo(Print& print);
    void printSensingTo(String& str);
    uint16_t readADC(uint8_t channel);
    DateTime getDateTime();
    SensorValues& getLastSensorData();
    void initGPS(Stream* gpsStream);
    NMEAGPS* getGPSObject();

private:
    NMEAGPS* gps = NULL;
    Stream* gpsStream = NULL;
    Adafruit_BME280* bme280 = NULL;             /**< Object of Adafruit BME280 sensor */
    ClosedCube_HDC1080* hdc1080 = NULL;         /**< Object of HDC1080 sensor */
    Adafruit_TSL2591* tsl2591 = NULL;           /**< Object of Adafruit TSL2591 sensor */
    CCS811* ccs811 = NULL;                      /**< Object of CCS811 sensor */
    LSM9DS1* imuSensor = NULL;                  /**< Object of LSM9DS1 sensor */
    GpioExpander* ioExpander = NULL;            /**< Object of GPIO Expander (SX1509) */
    MAX11609* max11609 = NULL;                  /**< Object of MAX11609 */
    RTC_DS3231* rtc = NULL;                     /**< Object of RTC sensor */

    SensorValues lastSensorData;                /**< Object of SensorValues struct. All sensor data are stored in this property */
    uint32_t lastSensorQuerryMs = 0;            /**< Records the time when the sensor data is read in milliseconds */

    void doAllSensing();
    void readBME280(float& T, float& P, float& H);
    void readHDC1080(float& T, float& H);
    void readTSL2591(double& lux);
    void configureTSL2591Sensor();
    void readGas(uint16_t& gas, uint16_t& co2);
    void readAccelerometer(float &ax, float &ay, float &az);
    void readMagnetometer(float &mx, float &my, float &mz, float &mH);
    void readGyro(float &gx, float &gy, float &gz);
    void readMagneticSensor(int& mag);
    void readWindSpeed(float& windspeed);
    void readGPS(gps_fix& fix);
};

#endif
