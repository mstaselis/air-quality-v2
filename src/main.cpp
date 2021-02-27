#include "main.h"

char firstLineBuffer[16];
char secondLineBuffer[16];

Task updateLCD(2000, UpdateLCD);
Task readCCS811(500, ReadCCS811Sensor);
Task readGasSensorData(1500, ReadGassSensorData);
Task readTemperatureAndHumidity(1500, ReadTemperatureAndHumidity);

LiquidCrystal_I2C lcd(LCD_ADDRESS, 20, 4);
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
  lcd.init();
  lcd.backlight();

  SoftTimer.add(&updateLCD);
  SoftTimer.add(&readCCS811);
  SoftTimer.add(&readGasSensorData);
  SoftTimer.add(&readTemperatureAndHumidity);
}

void UpdateLCD(Task *me)
{
  sprintf(firstLineBuffer, "T:%2d H:%2d P:%4d", (int)airQualityData.temperature, (int)(airQualityData.humidity >= 100 ? 99 : airQualityData.humidity), airQualityData.gasesPPM);
  lcd.setCursor(0, 0);
  lcd.print(firstLineBuffer);

  sprintf(secondLineBuffer, "CO2:%4d VO:%4d", airQualityData.co2, airQualityData.tvoc);
  lcd.setCursor(0, 1);
  lcd.print(secondLineBuffer);
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
}
