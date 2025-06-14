#ifndef LORA_TRANSMITTER_H
#define LORA_TRANSMITTER_H

#include <RadioLib.h>
#include "ATPC.h"

class LoRaTransmitter {
private:
    STM32WLx* radio;
    ATPC* atpc;
    
    // Configuration constants
    static const int ACK_TIMEOUT_MS = 200;
    static const int MAX_TX_RETRIES = 3;
    static const int ACK_SIZE = 7;
    
    // Helper methods
    bool validateAck(const uint8_t* ack, const uint8_t* payload);
    float extractRssiFromAck(const uint8_t* ack);
    void printAckData(const uint8_t* ackBuffer);
    
public:
    LoRaTransmitter(STM32WLx* radioInstance, ATPC* atpcInstance);
    
    // Main transmission method
    bool transmitWithAck(const uint8_t* payload, size_t payloadSize);
    
    // Configuration methods
    void setAckTimeout(int timeoutMs);
    void setMaxRetries(int retries);
};

#endif