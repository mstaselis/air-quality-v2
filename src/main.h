#include <Arduino.h>
#include <Wire.h>
#include <CCS811Sensor.h>
#include <DHT.h>
#include <SoftTimer.h>

#define SENSOR_ADDRESS 0x5B
#define DHTTYPE DHT22
#define INT_PIN 6
#define WAKE_PIN 7
#define TEMP_SENSOR_PIN 5
#define GAS_SENSOR_PIN A7

struct AirQualityData
{
    float temperature = 0;
    float humidity = 0;
    int ppm = 0;
    int co2 = 0;
    int tvoc = 0;
    int gasesPPM;
};

void UpdateSerial(Task *me);
void ReadCCS811Sensor(Task *me);
void ReadGassSensorData(Task *me);
void ReadTemperatureAndHumidity(Task *me);