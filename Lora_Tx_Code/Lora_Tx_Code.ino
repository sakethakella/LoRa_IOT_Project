#include <RadioLib.h>
#include <STM32RTC.h>
#include <STM32LowPower.h>
#include <Wire.h>
#include "SHT41_Sensor.h"
#include "ATPC.h"
#include "LoRaTransmitter.h"
#include "PayloadBuilder.h"
#include "DeepSleep.h"


// Configuration
#define I2C_SDA_PIN PA15
#define I2C_SCL_PIN PB15
#define SLEEP_MINUTES 1

// System Components
SHT41_Sensor sht41;
STM32WLx radio = new STM32WLx_Module();
ATPC atpc(&radio);
LoRaTransmitter loraTransmitter(&radio, &atpc);
PayloadBuilder payloadBuilder(&sht41);
DeepSleep deepSleep(&radio);

// RF Switch Configuration
const uint32_t rfSwitchPins[5] = {PA4, PA5, RADIOLIB_NC, RADIOLIB_NC, RADIOLIB_NC};
const Module::RfSwitchMode_t rfSwitchTable[] = {
  {STM32WLx::MODE_IDLE, {LOW, LOW}}, {STM32WLx::MODE_RX, {HIGH, LOW}}, 
  {STM32WLx::MODE_TX_HP, {LOW, HIGH}}, END_OF_MODE_TABLE
};

void initSystem() {
  Serial.begin(115200);
  Wire.begin(I2C_SDA_PIN, I2C_SCL_PIN);
  analogReadResolution(12);
  
  if (!sht41.begin() && !sht41.reinitialize()) Serial.println(F("SHT41 failed!"));
  
  radio.setRfSwitchTable(rfSwitchPins, rfSwitchTable);
  if (radio.begin(868.0, 125.0, 7, 5, RADIOLIB_SX126X_SYNC_WORD_PRIVATE, 
                  atpc.getCurrentTxPower(), 8, 1.7, false) != RADIOLIB_ERR_NONE) {
    Serial.println(F("LoRa init failed!"));
  }
}

void setup() {
  Serial.begin(115200);
  while (!Serial && millis() < 3000);
  Serial.println(F("LoRa Transmitter Ready!"));
  
  deepSleep.begin();
  
  // Set RTC only on first run
  if (deepSleep.getIsFirstRun()) {
    STM32RTC::getInstance().begin();
    STM32RTC::getInstance().setTime(13, 25, 0);
    STM32RTC::getInstance().setDate(28, 5, 25);
  }
  
  initSystem();
}

void loop() {
  uint8_t payload[PayloadBuilder::PAYLOAD_SIZE];
  payloadBuilder.buildPayload(payload);
  loraTransmitter.transmitWithAck(payload, PayloadBuilder::PAYLOAD_SIZE);
  
  deepSleep.sleepMinutes(SLEEP_MINUTES);
  initSystem(); // Reinitialize after wakeup
}