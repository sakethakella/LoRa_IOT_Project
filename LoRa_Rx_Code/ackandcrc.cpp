#include "ackandcrc.h"

bool crccheck(uint8_t *buf) {
  uint8_t crc = 0;
  for (int i = 0; i < 15; i++) {
    crc = crc + buf[i];
  }
  return (crc == buf[15]);
}

void send_ack(uint8_t *buffer,uint8_t *ack,float receivedRssi){
    ack[0] = 0x55; 
    for (int i = 1; i <= 4; i++) {
        ack[i] = buffer[i];
    }
    int16_t rssiInt = (int16_t)(receivedRssi * 10.0);
    ack[5] = (rssiInt >> 8) & 0xFF; 
    ack[6] = rssiInt & 0xFF;        
}
