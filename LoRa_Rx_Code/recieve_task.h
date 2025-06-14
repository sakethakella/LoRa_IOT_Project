#ifndef RECIEVE_TASK_H
#define RECIEVE_TASK_H

#include <Arduino.h>
#include <RadioLib.h>
#include <FreeRTOS.h>
#include <queue.h>
#include "ackandcrc.h"   // for crccheck() and send_ack()

// packet length constants
#define PACKET_BUFFER_LEN 17
#define ACK_PACKET_LEN     7

// the struct you’re sending over the FreeRTOS queue
typedef struct {
  uint8_t buffer[PACKET_BUFFER_LEN];
  float   rssi;
} PacketMsg;

extern STM32WLx radio;
extern QueueHandle_t packetQueue;

// task prototype
void ReceiveTask(void *pvParameters);
void PrintTask(void *pvParameters);
void WatchdogTask(void *pvParameters);

#endif // RECEIVE_TASK_H
