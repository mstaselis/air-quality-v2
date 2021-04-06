#include <SparkFunCCS811.h>

struct CCS811SensorData
{
    uint16_t co2;
    uint16_t tvoc;
};

class CCS811Sensor
{
    private:
        uint8_t address;
        uint8_t mode;
        CCS811* ccs811;        
        
    public:
    CCS811Sensor(uint8_t address, uint8_t mode = 2);    
    ~CCS811Sensor();  

    bool Init(); 
    CCS811SensorData GetSensorData();     
};