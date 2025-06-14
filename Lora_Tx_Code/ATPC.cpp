#include "ATPC.h"
#include <Arduino.h>

ATPC::ATPC(STM32WLx* radioInstance, int initialTxPower, float targetRssi, 
           int minPower, int maxPower) {
    radio = radioInstance;
    currentTxPower = initialTxPower;
    reqRssi = targetRssi;
    minTxPower = minPower;
    maxTxPower = maxPower;
    alpha = 0;
}

void ATPC::begin(STM32WLx* radioInstance) {
    radio = radioInstance;
}

void ATPC::setAlpha(float error) {
    float absError = abs(error);

    if (absError >= 50) {
        alpha = 0.1;    // largest step
    } else if (absError >= 20) {
        alpha = 0.08;   // medium step
    } else if (absError >= 10) {
        alpha = 0.06;   // smaller step
    } else if (absError >= 5) {
        alpha = 0.02;   // fine tuning
    } else {
        alpha = 0;      // no adjustment
    }
}

void ATPC::adjustTransmitPower(float feedbackRssi) {
    if (!radio) {
        Serial.println("ATPC: Radio not initialized!");
        return;
    }

    float error = feedbackRssi - reqRssi;  // Positive: signal too strong

    setAlpha(error);

    if (alpha > 0) {
        float deltaPower = -alpha * error;
        int tempPower = currentTxPower + round(deltaPower);
        currentTxPower = constrain(tempPower, minTxPower, maxTxPower);

        int state = radio->setOutputPower(currentTxPower);  

        if (state == RADIOLIB_ERR_NONE) {          
            Serial.print("RSSI Value: ");
            Serial.println(feedbackRssi);
            Serial.print("Output Tx power set to: ");    
            Serial.println(currentTxPower);
        } else {
            Serial.println("ATPC: Failed to set Tx power!");
        }
    }
    // else do nothing, power is good enough
}

int ATPC::getCurrentTxPower() const {
    return currentTxPower;
}

void ATPC::setTargetRssi(float targetRssi) {
    reqRssi = targetRssi;
}

void ATPC::setPowerLimits(int minPower, int maxPower) {
    minTxPower = minPower;
    maxTxPower = maxPower;
    // Ensure current power is within new limits
    currentTxPower = constrain(currentTxPower, minTxPower, maxTxPower);
}

void ATPC::resetToInitialPower() {
    currentTxPower = 22; // Default initial power
    if (radio) {
        radio->setOutputPower(currentTxPower);
    }
}