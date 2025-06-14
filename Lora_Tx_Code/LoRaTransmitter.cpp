#include "LoRaTransmitter.h"

LoRaTransmitter::LoRaTransmitter(STM32WLx* radioInstance, ATPC* atpcInstance) 
    : radio(radioInstance), atpc(atpcInstance) {
}

bool LoRaTransmitter::validateAck(const uint8_t* ack, const uint8_t* payload) {
    return (ack[0] == 0x55 && 
            ack[1] == payload[1] && ack[2] == payload[2] && 
            ack[3] == payload[3] && ack[4] == payload[4]);
}

float LoRaTransmitter::extractRssiFromAck(const uint8_t* ack) {
    int16_t rawRssi = (int16_t)((ack[5] << 8) | ack[6]);
    return (float)rawRssi / 10.0;
}

void LoRaTransmitter::printAckData(const uint8_t* ackBuffer) {
    Serial.print("ACK: ");
    for (int i = 0; i < ACK_SIZE; i++) {
        if (ackBuffer[i] < 0x10) Serial.print("0");
        Serial.print(ackBuffer[i], HEX);
        Serial.print(" ");
    }
    Serial.println();
}

bool LoRaTransmitter::transmitWithAck(const uint8_t* payload, size_t payloadSize) {
    for (int attempt = 0; attempt < MAX_TX_RETRIES; attempt++) {
        Serial.print("TX attempt ");
        Serial.println(attempt + 1);
        
        // Transmit payload
        if (radio->transmit(payload, payloadSize) != RADIOLIB_ERR_NONE) {
            Serial.println("TX failed");
            continue;
        }
        
        Serial.println("Payload sent, waiting for ACK...");
        radio->startReceive();
        
        unsigned long startTime = millis();
        uint8_t ackBuffer[ACK_SIZE] = {0};
        
        // Wait for ACK with timeout
        while (millis() - startTime < ACK_TIMEOUT_MS) {
            if (radio->getPacketLength() == ACK_SIZE) {
                if (radio->readData(ackBuffer, ACK_SIZE) == RADIOLIB_ERR_NONE) {
                    printAckData(ackBuffer);
                    
                    if (validateAck(ackBuffer, payload)) {
                        Serial.println("Valid ACK received!");
                        float rssi = extractRssiFromAck(ackBuffer);
                        atpc->adjustTransmitPower(rssi);
                        radio->standby();
                        return true;
                    } else {
                        Serial.println("Invalid ACK");
                    }
                }
                radio->startReceive();
            }
            delay(1);
        }
        
        radio->standby();
        if (attempt < MAX_TX_RETRIES - 1) {
            Serial.println("Retrying...");
        }
    }
    
    Serial.println("Max retries reached");
    return false;
}

void LoRaTransmitter::setAckTimeout(int timeoutMs) {
    const_cast<int&>(ACK_TIMEOUT_MS) = timeoutMs;
}

void LoRaTransmitter::setMaxRetries(int retries) {
    const_cast<int&>(MAX_TX_RETRIES) = retries;
}