#ifndef PAYLOAD_BUILDER_H
#define PAYLOAD_BUILDER_H

#include <Arduino.h>
#include <STM32RTC.h>
#include <time.h>
#include "SHT41_Sensor.h"

class PayloadBuilder {
public:
    static const uint8_t PAYLOAD_SIZE = 17;
    static const uint8_t HEADER_BYTE = 0xAA;
    static const uint8_t FOOTER_BYTE = 0xFF;
    
    PayloadBuilder(SHT41_Sensor* sensor);
    
    void buildPayload(uint8_t* payload);
    
private:
    SHT41_Sensor* _sensor;
    STM32RTC* _rtc;
    
    uint32_t getUnixTimestamp();
    float readBatteryVoltage();
    uint8_t calculateChecksum(const uint8_t* payload, uint8_t length);
    void insertDeviceId(uint8_t* payload, uint8_t startIndex);
    void insertSensorData(uint8_t* payload, uint8_t startIndex);
    void insertBatteryData(uint8_t* payload, uint8_t startIndex);
    void insertTimestamp(uint8_t* payload, uint8_t startIndex);
};

#endif