#include <Arduino.h>
#include "CCS811Sensor.h"

CCS811Sensor::CCS811Sensor(uint8_t address, uint8_t mode)
{
    this->address = address;
    this->mode = mode;
    this->ccs811 = new CCS811(this->address);
}

void CCS811Sensor::Init()
{
    CCS811Core::CCS811_Status_e status = ccs811->beginWithStatus();
    delay(1500);    
    ccs811->setDriveMode(this->mode);    
    ccs811->enableInterrupts();    
}

CCS811SensorData CCS811Sensor::GetSensorData()
{
    CCS811SensorData data;
    ccs811->readAlgorithmResults();

    data.co2 = ccs811->getCO2();
    data.tvoc = ccs811->getTVOC();

    return data;
}



CCS811Sensor::~CCS811Sensor()
{
    if (this->ccs811 != NULL)
    {
        delete this->ccs811;
    }
}
