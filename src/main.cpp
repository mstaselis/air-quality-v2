#include <MKRWAN.h>

#include "main.h"
#include "arduino_secrets.h"
#include "LoraMessage.h"

char firstLineBuffer[17];
char secondLineBuffer[17];

LiquidCrystal_I2C lcd(LCD_ADDRESS, 20, 4);

Task readCCS811(500, ReadCCS811Sensor);
Task readGasSensorData(1500, ReadGassSensorData);
Task readTemperatureAndHumidity(1500, ReadTemperatureAndHumidity);
Task updateLCD(2000, UpdateLCD);
Task sendToTTN(1800000, SendToTTN);

CCS811Sensor ccs811Sensor(SENSOR_ADDRESS);
DHT dht(TEMP_SENSOR_PIN, DHTTYPE);
AirQualityData airQualityData;
LoRaModem modem;

void setup()
{
  Serial.begin(9600);

  Wire.begin();
  dht.begin();
  ccs811Sensor.Init();
  lcd.init();
  lcd.backlight();

  if (!modem.begin(EU868))
  {
    lcd.setCursor(0, 0);
    lcd.print("Failed to start module");
    while (1)
    {
    }
  };

  int connected = modem.joinOTAA(SECRET_APP_EUI, SECRET_APP_KEY);
  if (!connected)
  {
    lcd.setCursor(0, 0);
    lcd.print("Something went wrong; are you indoor? Move near a window and retry");
    while (1)
    {
    }
  }

  SoftTimer.add(&readGasSensorData);
  SoftTimer.add(&readTemperatureAndHumidity);
  SoftTimer.add(&readCCS811);
  SoftTimer.add(&updateLCD);
  SoftTimer.add(&sendToTTN);

  digitalWrite(WAKE_PIN, 1);

  modem.dataRate(5);
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

void SendToTTN(Task *me)
{
  LoraMessage message;
  message.addTemperature(airQualityData.temperature);
  message.addHumidity(airQualityData.humidity);
  message.addUint16(airQualityData.co2);
  message.addUint16(airQualityData.tvoc);
  message.addUint16(airQualityData.gasesPPM);

  int err;
  modem.beginPacket();
  modem.write(message.getBytes(), message.getLength());

  err = modem.endPacket(false);
  if (err > 0)
  {
    Serial.println("Message sent correctly!");
  }
  else
  {
    Serial.println("Error");
  }

  Serial.println("Sent");
  delete &message;
}