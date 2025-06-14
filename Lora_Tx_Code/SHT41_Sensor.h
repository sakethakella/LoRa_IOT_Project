#ifndef SHT41_SENSOR_H
#define SHT41_SENSOR_H

#include <Adafruit_SHT4x.h>
#include <Wire.h>

class SHT41_Sensor {
private:
    Adafruit_SHT4x sht4;
    bool initialized;

public:
    // Constructor
    SHT41_Sensor();
    
    // Initialize the sensor
    bool begin();
    
    // Read temperature and humidity
    bool readSensor(float &temperature, float &humidity);
    
    // Get temperature and humidity as integers for payload
    bool getPayloadData(uint16_t &tempInt, uint16_t &humidityInt);
    
    // Check if sensor is initialized
    bool isInitialized() const;
    
    // Reinitialize sensor (useful after wake from sleep)
    bool reinitialize();
};

#endif // SHT41_SENSOR_H