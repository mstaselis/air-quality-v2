#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <CCS811Sensor.h>
#include <DHT.h>
#include <SoftTimer.h>

#define LCD_ADDRESS 0x3F
#define SENSOR_ADDRESS 0x5B
#define DHTTYPE DHT11
#define INT_PIN 7
#define WAKE_PIN 8
#define TEMP_SENSOR_PIN 6
#define GAS_SENSOR_PIN A3

struct AirQualityData
{
    float temperature = 0;
    float humidity = 0;
    int ppm = 0;
    int co2 = 0;
    int tvoc = 0;
    int gasesPPM;
};

void UpdateLCD(Task *me);
void SendToTTN(Task *me);
void ReadCCS811Sensor(Task *me);
void ReadGassSensorData(Task *me);
void ReadTemperatureAndHumidity(Task *me);