#include <STM32FreeRTOS.h>
#include <RadioLib.h>
#include "recieve_task.h"

STM32WLx radio = new STM32WLx_Module();
QueueHandle_t packetQueue;

void setup() {
  Serial.begin(115200);
  static const uint32_t rfswitch_pins[5] = {PA4, PA5, RADIOLIB_NC, RADIOLIB_NC, RADIOLIB_NC};
  static const Module::RfSwitchMode_t rfswitch_table[] = {
    {STM32WLx::MODE_IDLE,   {LOW,  LOW}},
    {STM32WLx::MODE_RX,     {HIGH, LOW}},
    {STM32WLx::MODE_TX_HP,  {LOW,  HIGH}},
    END_OF_MODE_TABLE
  };
  radio.setRfSwitchTable(rfswitch_pins, rfswitch_table);
  int16_t state = radio.begin(868.0,125.0,7,5,RADIOLIB_SX126X_SYNC_WORD_PRIVATE,14,8,1.7,false);

  if (state != RADIOLIB_ERR_NONE) {
    Serial.begin(115200);
    while (!Serial) { }
    Serial.print("LoRa init failed, code ");
    Serial.println(state);
    while (true) { } // Halt if radio initialization fails
  }

  Serial.println("LoRa Receiver Ready!");
  packetQueue = xQueueCreate(30, sizeof(PacketMsg)); // Queue can hold 5 PacketMsg structs
  if (packetQueue == NULL) {
    Serial.begin(115200);
    while (!Serial) { }
    Serial.println("Failed to create packetQueue.");
    while (true) { } 
  }
  IWDG->KR = 0x5555;
  IWDG->PR = 0x04; 
  IWDG->RLR = 1000;
  IWDG->KR = 0xAAAA;
  IWDG->KR = 0xCCCC;
  xTaskCreate(ReceiveTask,"ReceiveTask",512,nullptr,2,nullptr);
  xTaskCreate(PrintTask,"PrintTask",384,nullptr, 1,nullptr);
  xTaskCreate(WatchdogTask,"WatchdogTask",256,nullptr,0, nullptr);
  vTaskStartScheduler();
  for (;;) { }
}

void loop() {
}
 
 