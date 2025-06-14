#include "SHT41_Sensor.h"

// Constructor
SHT41_Sensor::SHT41_Sensor() : initialized(false) {
    // Constructor body - initialization handled in begin()
}

// Initialize the sensor
bool SHT41_Sensor::begin() {
    if (sht4.begin()) {
        initialized = true;
        return true;
    } else {
        initialized = false;
        return false;
    }
}

// Read temperature and humidity as float values
bool SHT41_Sensor::readSensor(float &temperature, float &humidity) {
    if (!initialized) {
        return false;
    }
    
    sensors_event_t humidity_event, temp_event;
    sht4.getEvent(&humidity_event, &temp_event);
    
    // Check for valid readings (basic validation)
    if (temp_event.temperature < -40 || temp_event.temperature > 125) {
        return false; // Temperature out of sensor range
    }
    
    if (humidity_event.relative_humidity < 0 || humidity_event.relative_humidity > 100) {
        return false; // Humidity out of valid range
    }
    
    temperature = temp_event.temperature;
    humidity = humidity_event.relative_humidity;
    
    return true;
}

// Get temperature and humidity formatted for payload
bool SHT41_Sensor::getPayloadData(uint16_t &tempInt, uint16_t &humidityInt) {
    float temperature, humidity;
    
    if (!readSensor(temperature, humidity)) {
        // Return error values if sensor read fails
        tempInt = 0;
        humidityInt = 0;
        return false;
    }
    
    // Convert temperature to integer (multiply by 100 for 2 decimal precision)
    tempInt = (uint16_t)(temperature * 100);
    
    // Convert humidity to integer (multiply by 10 for 1 decimal precision)
    humidityInt = (uint16_t)(humidity * 10);
    
    return true;
}

// Check if sensor is initialized
bool SHT41_Sensor::isInitialized() const {
    return initialized;
}

// Reinitialize sensor (useful after wake from sleep)
bool SHT41_Sensor::reinitialize() {
    initialized = false;
    return begin();
}