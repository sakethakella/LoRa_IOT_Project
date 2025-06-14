#include "PayloadBuilder.h"

// Hardware Configuration
#define VREF_CAL_ADDR       ((uint16_t*)0x1FFF75AA)
#define DEVICE_ID_ADDR      ((uint32_t*)0x1FFF7590)
#define BATTERY_ADC_PIN     PA10

PayloadBuilder::PayloadBuilder(SHT41_Sensor* sensor) : _sensor(sensor) {
    _rtc = &STM32RTC::getInstance();
}

void PayloadBuilder::buildPayload(uint8_t* payload) {
    uint8_t index = 0;
    
    // Header
    payload[index++] = HEADER_BYTE;
    
    // Device ID (4 bytes)
    insertDeviceId(payload, index);
    index += 4;
    
    // Sensor data (4 bytes)
    insertSensorData(payload, index);
    index += 4;
    
    // Battery voltage (2 bytes)
    insertBatteryData(payload, index);
    index += 2;
    
    // Timestamp (4 bytes)
    insertTimestamp(payload, index);
    index += 4;
    
    // Checksum (1 byte)
    payload[index] = calculateChecksum(payload, index);
    index++;
    
    // Footer
    payload[index] = FOOTER_BYTE;
}

void PayloadBuilder::insertDeviceId(uint8_t* payload, uint8_t startIndex) {
    uint32_t deviceId = *DEVICE_ID_ADDR;
    payload[startIndex]     = (deviceId >> 24) & 0xFF;
    payload[startIndex + 1] = (deviceId >> 16) & 0xFF;
    payload[startIndex + 2] = (deviceId >> 8) & 0xFF;
    payload[startIndex + 3] = deviceId & 0xFF;
}

void PayloadBuilder::insertSensorData(uint8_t* payload, uint8_t startIndex) {
    uint16_t temperature, humidity;
    _sensor->getPayloadData(temperature, humidity);
    
    payload[startIndex]     = (temperature >> 8) & 0xFF;
    payload[startIndex + 1] = temperature & 0xFF;
    payload[startIndex + 2] = (humidity >> 8) & 0xFF;
    payload[startIndex + 3] = humidity & 0xFF;
}

void PayloadBuilder::insertBatteryData(uint8_t* payload, uint8_t startIndex) {
    uint16_t batteryVoltage = (uint16_t)readBatteryVoltage();
    payload[startIndex]     = (batteryVoltage >> 8) & 0xFF;
    payload[startIndex + 1] = batteryVoltage & 0xFF;
}

void PayloadBuilder::insertTimestamp(uint8_t* payload, uint8_t startIndex) {
    uint32_t timestamp = getUnixTimestamp();
    payload[startIndex]     = (timestamp >> 24) & 0xFF;
    payload[startIndex + 1] = (timestamp >> 16) & 0xFF;
    payload[startIndex + 2] = (timestamp >> 8) & 0xFF;
    payload[startIndex + 3] = timestamp & 0xFF;
}

uint8_t PayloadBuilder::calculateChecksum(const uint8_t* payload, uint8_t length) {
    uint8_t checksum = 0;
    for (uint8_t i = 0; i < length; i++) {
        checksum += payload[i];
    }
    return checksum;
}

uint32_t PayloadBuilder::getUnixTimestamp() {
    struct tm timeStruct = {0};
    timeStruct.tm_year = 2000 + _rtc->getYear() - 1900;
    timeStruct.tm_mon = _rtc->getMonth() - 1;
    timeStruct.tm_mday = _rtc->getDay();
    timeStruct.tm_hour = _rtc->getHours();
    timeStruct.tm_min = _rtc->getMinutes();
    timeStruct.tm_sec = _rtc->getSeconds();
    return (uint32_t)mktime(&timeStruct);
}

float PayloadBuilder::readBatteryVoltage() {
    pinMode(BATTERY_ADC_PIN, INPUT_ANALOG);
    
    // Optimized averaging with reduced iterations
    uint32_t sensorSum = 0, vrefSum = 0;
    const uint8_t samples = 8; // Reduced from 16 for faster execution
    
    for (uint8_t i = 0; i < samples; i++) {
        sensorSum += analogRead(BATTERY_ADC_PIN);
        vrefSum += analogRead(AVREF);
        delayMicroseconds(50); // Reduced delay
    }
    
    uint16_t sensorAvg = sensorSum / samples;
    uint16_t vrefAvg = vrefSum / samples;
    uint16_t vrefCal = *VREF_CAL_ADDR;
    
    // Simple validation
    if (vrefAvg <= 100 || vrefAvg >= 4000) {
        return 0.0; // Return 0 for invalid readings
    }
    
    float vdda = (3300.0 * vrefCal) / vrefAvg;
    return (sensorAvg / 4095.0) * vdda;
}