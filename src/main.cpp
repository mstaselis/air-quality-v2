#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <SparkFunCCS811.h>
#include <DHT.h>

#define LCD_ADDRESS 0x3F
#define SENSOR_ADDRESS 0x5B
#define DHTTYPE DHT11

#define INT_PIN 6
#define WAKE_PIN 7
#define TEMP_SENSOR_PIN 5
#define GAS_SENSOR_PIN A7

uint16_t co2 = 0;
uint16_t tvoc = 0;
char firstLineBuffer[16];
char secondLineBuffer[16];

LiquidCrystal_I2C lcd(LCD_ADDRESS, 20, 4);
CCS811 mySensor(SENSOR_ADDRESS);
DHT dht(TEMP_SENSOR_PIN, DHTTYPE);

void prepareFirstPrintBuffer(float temperature, float humidity, int ppm)
{
  sprintf(firstLineBuffer, "T:%2d H:%2d P:%4d", (int)temperature, (int)(humidity >= 100 ? 99 : humidity), ppm);
}

void prepareSecondPrintBuffer()
{
  sprintf(secondLineBuffer, "CO2:%4d VO:%4d", co2, tvoc);
}

void setup()
{
  Wire.begin();
  Serial.begin(9600);

  mySensor.beginWithStatus();

  mySensor.setDriveMode(2);
  Serial.println("Mode completed");
  mySensor.enableInterrupts();
  Serial.println("Interrupt configured");
  digitalWrite(WAKE_PIN, 1);

  dht.begin();

  lcd.init();
  lcd.backlight();
}

void loop()
{
  //lcd.clear();

  if (digitalRead(INT_PIN) == 0)
  {
    digitalWrite(WAKE_PIN, 0);
    delay(1);

    mySensor.readAlgorithmResults();
    co2 = mySensor.getCO2();
    tvoc = mySensor.getTVOC();

    digitalWrite(WAKE_PIN, 1);
    delay(1);
  }

  int gasData = analogRead(GAS_SENSOR_PIN);
  float h = dht.readHumidity();
  float t = dht.readTemperature();  


  prepareFirstPrintBuffer(t, h, gasData); 
  lcd.setCursor(0, 0);
  lcd.print(firstLineBuffer);   
  
  prepareSecondPrintBuffer();
  lcd.setCursor(0, 1);
  lcd.print(secondLineBuffer);   
  delay(2000);
}
