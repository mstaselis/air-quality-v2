#include "main.h"

char firstLineBuffer[16];
char secondLineBuffer[16];

Task serialUpdate(2000, UpdateSerial);
Task readCCS811(500, ReadCCS811Sensor);
Task readGasSensorData(1500, ReadGassSensorData);
Task readTemperatureAndHumidity(1500, ReadTemperatureAndHumidity);

CCS811Sensor ccs811Sensor(SENSOR_ADDRESS);
DHT dht(TEMP_SENSOR_PIN, DHTTYPE);
AirQualityData airQualityData;

void setup()
{
  Wire.begin();
  Serial.begin(9600);
  ccs811Sensor.Init();

  digitalWrite(WAKE_PIN, 1);

  dht.begin();

  SoftTimer.add(&serialUpdate);
  SoftTimer.add(&readCCS811);
  SoftTimer.add(&readGasSensorData);
  SoftTimer.add(&readTemperatureAndHumidity);
}

void UpdateSerial(Task *me)
{
  sprintf(firstLineBuffer, "T:%2d H:%2d P:%4d", (int)airQualityData.temperature, (int)(airQualityData.humidity >= 100 ? 99 : airQualityData.humidity), airQualityData.gasesPPM);
  Serial.println(firstLineBuffer);

  sprintf(secondLineBuffer, "CO2:%4d VO:%4d", airQualityData.co2, airQualityData.tvoc); 
  Serial.println(secondLineBuffer);
}

void ReadCCS811Sensor(Task *me)
{
  if (digitalRead(INT_PIN) == 0)
  {
    digitalWrite(WAKE_PIN, 0);
    delay(1);

    CCS811SensorData data = ccs811Sensor.GetSensorData();
    airQualityData.co2 = data.co2;
    airQualityData.tvoc = data.tvoc;

    digitalWrite(WAKE_PIN, 1);
    delay(1);
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

  ccs811Sensor.SetBaseData(airQualityData.temperature);
}
