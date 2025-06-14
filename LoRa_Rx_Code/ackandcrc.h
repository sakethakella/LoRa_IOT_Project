#ifndef ACKANDCRC_H
#define ACKANDCRC_H

#include <cstdint> 

bool crccheck(uint8_t *buf);
void send_ack(uint8_t *buffer,uint8_t *ack,float receivedRssi);

#endif