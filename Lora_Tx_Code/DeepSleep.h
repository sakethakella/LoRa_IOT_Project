#ifndef DEEPSLEEP_H
#define DEEPSLEEP_H

#include <Arduino.h>
#include <STM32LowPower.h>
#include <RadioLib.h>

class DeepSleep {
private:
    STM32WLx* radio;
    static uint32_t wakeupCounter;
    static bool isFirstRun;
    
    void configureLowPowerPins();
    void (*reinitCallback)();

public:
    // Constructor
    DeepSleep(STM32WLx* radioPtr, void (*callback)() = nullptr);
    
    // Initialize the deep sleep system
    void begin();
    
    // Enter deep sleep for specified duration in milliseconds
    void sleep(uint32_t durationMs);
    
    // Enter deep sleep for specified duration in seconds
    void sleepSeconds(uint32_t durationSeconds);
    
    // Enter deep sleep for specified duration in minutes
    void sleepMinutes(uint32_t durationMinutes);
    
    // Get current wakeup counter
    uint32_t getWakeupCounter() const;
    
    // Check if this is the first run after power-on
    bool getIsFirstRun() const;
    
    // Set reinitialization callback function
    void setReinitCallback(void (*callback)());
};

#endif // DEEPSLEEP_H