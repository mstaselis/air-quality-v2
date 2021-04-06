#include <MKRWAN.h>

#include "main.h"
#include "arduino_secrets.h"
#include "LoraMessage.h"

#define LED 5

Task readCCS811(500, ReadCCS811Sensor);
Task readGasSensorData(5000, ReadGassSensorData);
Task readTemperatureAndHumidity(5000, ReadTemperatureAndHumidity);
Task sendToTTN(1200000, SendToTTN);

CCS811Sensor ccs811Sensor(SENSOR_ADDRESS);
DHT dht(TEMP_SENSOR_PIN, DHTTYPE);
AirQualityData airQualityData;
LoRaModem modem;

void setup()
{
  Serial.begin(9600);
  pinMode(LED, OUTPUT);

  Wire.begin();
  dht.begin();
  bool init = ccs811Sensor.Init();

  if (!init)
  {
    CriticalError();
  }

  if (!modem.begin(EU868))
  {
    Serial.println("Failed to start module");
    CriticalError();
  };

  int connected = modem.joinOTAA(SECRET_APP_EUI, SECRET_APP_KEY);
  if (!connected)
  {
    Serial.println("Something went wrong; are you indoor? Move near a window and retry");
    CriticalError();
  }

  SoftTimer.add(&readGasSensorData);
  SoftTimer.add(&readTemperatureAndHumidity);
  SoftTimer.add(&readCCS811);
  SoftTimer.add(&sendToTTN);

  digitalWrite(WAKE_PIN, 1);

  modem.dataRate(5);
}

void ReadCCS811Sensor(Task *me)
{
  if (digitalRead(INT_PIN) == 0)
  {
    digitalWrite(WAKE_PIN, 0);
    delay(10);

    CCS811SensorData data = ccs811Sensor.GetSensorData();
    airQualityData.co2 = data.co2;
    airQualityData.tvoc = data.tvoc;

    digitalWrite(WAKE_PIN, 1);
    delay(10);
  }
}

void ReadGassSensorData(Task *me)
{
  airQualityData.gasesPPM = analogRead(GAS_SENSOR_PIN);
}

void ReadTemperatureAndHumidity(Task *me)
{
  airQualityData.humidity = dht.readHumidity();
  airQualityData.temperature = dht.readTemperature();
}

void SendToTTN(Task *me)
{
  LoraMessage message;
  message.addTemperature(airQualityData.temperature);
  message.addHumidity(airQualityData.humidity);
  message.addUint16(airQualityData.co2);
  message.addUint16(airQualityData.tvoc);
  message.addUint16(airQualityData.gasesPPM);

  modem.beginPacket();
  modem.write(message.getBytes(), message.getLength());

  modem.endPacket(false);
}

void CriticalError()
{
  while (1)
  {
    digitalWrite(LED, HIGH);
    delay(500);
    digitalWrite(LED, LOW);
    delay(500);    
  }
}