#include "DeepSleep.h"

// Static member definitions
uint32_t DeepSleep::wakeupCounter = 0;
bool DeepSleep::isFirstRun = true;

DeepSleep::DeepSleep(STM32WLx* radioPtr, void (*callback)()) 
    : radio(radioPtr), reinitCallback(callback) {
}

void DeepSleep::begin() {
    LowPower.begin();
}

void DeepSleep::configureLowPowerPins() {
    // Set unused pins to INPUT_ANALOG for lowest power consumption
    const uint8_t excludedPins[] = {4, 5, 10, 15, 19, 31}; // Active pins
    const uint8_t excludedCount = sizeof(excludedPins) / sizeof(excludedPins[0]);
    
    for (uint8_t pin = 0; pin < 32; pin++) {
        bool excluded = false;
        for (uint8_t i = 0; i < excludedCount; i++) {
            if (pin == excludedPins[i]) {
                excluded = true;
                break;
            }
        }
        if (!excluded) {
            pinMode(pin, INPUT_ANALOG);
        }
    }
}

void DeepSleep::sleep(uint32_t durationMs) {
    // Prepare for sleep
    Serial.flush();
    
    // Put radio to sleep
    if (radio) {
        radio->sleep(true);
        delay(10);
    }
    
    // Configure pins for low power
    configureLowPowerPins();
    
    // Close serial connection
    Serial.end();
    delay(50);
    
    // Enter deep sleep
    LowPower.deepSleep(durationMs);
    
    // Update counters after wakeup
    wakeupCounter++;
    isFirstRun = false;
    
    // Reinitialize system if callback is provided
    if (reinitCallback) {
        reinitCallback();
    }
}

void DeepSleep::sleepSeconds(uint32_t durationSeconds) {
    sleep(durationSeconds * 1000);
}

void DeepSleep::sleepMinutes(uint32_t durationMinutes) {
    sleep(durationMinutes * 60 * 1000);
}

uint32_t DeepSleep::getWakeupCounter() const {
    return wakeupCounter;
}

bool DeepSleep::getIsFirstRun() const {
    return isFirstRun;
}

void DeepSleep::setReinitCallback(void (*callback)()) {
    reinitCallback = callback;
}