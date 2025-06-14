#ifndef ATPC_H
#define ATPC_H

#include <RadioLib.h>

class ATPC {
private:
    int currentTxPower;
    float reqRssi;
    int minTxPower;
    int maxTxPower;
    float alpha;
    STM32WLx* radio;

    void setAlpha(float error);

public:
    // Constructor
    ATPC(STM32WLx* radioInstance, int initialTxPower = 22, float targetRssi = -90.0, 
         int minPower = -9, int maxPower = 22);
    
    // Initialize ATPC with radio instance
    void begin(STM32WLx* radioInstance);
    
    // Adjust transmit power based on RSSI feedback
    void adjustTransmitPower(float feedbackRssi);
    
    // Get current transmit power
    int getCurrentTxPower() const;
    
    // Set target RSSI
    void setTargetRssi(float targetRssi);
    
    // Set power limits
    void setPowerLimits(int minPower, int maxPower);
    
    // Reset to initial power
    void resetToInitialPower();
};

#endif // ATPC_H