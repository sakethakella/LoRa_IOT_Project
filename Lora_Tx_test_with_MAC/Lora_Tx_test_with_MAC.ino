#include <RadioLib.h>

// Create the STM32WLx radio object
STM32WLx radio = new STM32WLx_Module();
const int MAX_RETRIES = 3;
const unsigned long ACK_TIMEOUT = 4000;  // 10 seconds

String getUniqueID() {
  uint32_t uid0 = *(uint32_t*)0x1FFF7590;
  uint32_t uid1 = *(uint32_t*)0x1FFF7594;
  uint32_t uid2 = *(uint32_t*)0x1FFF7598;

  char buffer[25];
  sprintf(buffer, "%08lX%08lX%08lX", uid0, uid1, uid2);
  return String(buffer);
}


void setup() {
  Serial.begin(115200);
  while (!Serial);

  // Configure RF switch for LoRa-E5
  static const uint32_t rfswitch_pins[5] = {PA4, PA5, RADIOLIB_NC, RADIOLIB_NC, RADIOLIB_NC};
  static const Module::RfSwitchMode_t rfswitch_table[] = {
    {STM32WLx::MODE_IDLE,  {LOW,  LOW}},
    {STM32WLx::MODE_RX,    {HIGH, LOW}},
    {STM32WLx::MODE_TX_HP, {LOW,  HIGH}},
    END_OF_MODE_TABLE
  };
  radio.setRfSwitchTable(rfswitch_pins, rfswitch_table);
  int state = radio.begin(868.0, 125.0, 9, 5, RADIOLIB_SX126X_SYNC_WORD_PRIVATE, 14, 8, 1.7, false);
  if (state != RADIOLIB_ERR_NONE) {
    Serial.print("LoRa init failed, code ");
    Serial.println(state);
    while (true);
  }

  Serial.println("Attempting to establish connection...");
  if (establishConnection()) {
    Serial.println("Connection established successfully.");
  } else {
    Serial.println("Failed to establish connection.");
  }
}

// Initialization flag
bool initialized = false;

void initializeRadio() {
  if (initialized) return;

  Serial.begin(115200);
  while (!Serial);

  // RF switch setup
  static const uint32_t rfswitch_pins[5] = {PA4, PA5, RADIOLIB_NC, RADIOLIB_NC, RADIOLIB_NC};
  static const Module::RfSwitchMode_t rfswitch_table[] = {
    {STM32WLx::MODE_IDLE,  {LOW,  LOW}},
    {STM32WLx::MODE_RX,    {HIGH, LOW}},
    {STM32WLx::MODE_TX_HP, {LOW,  HIGH}},
    END_OF_MODE_TABLE
  };
  radio.setRfSwitchTable(rfswitch_pins, rfswitch_table);

  // Initialize radio
  int state = radio.begin(868.0, 125.0, 9, 5, RADIOLIB_SX126X_SYNC_WORD_PRIVATE, 14, 8, 1.7, false);
  if (state != RADIOLIB_ERR_NONE) {
    Serial.print("LoRa init failed, code ");
    Serial.println(state);
    while (true);
  }

  Serial.println("LoRa Transmitter Initialized - 868 MHz, SF6, BW125kHz, CR4/5");
  initialized = true;
}
  
bool establishConnection() {
  initializeRadio();
  String macID = getUniqueID();
  String message = "HELLO\n" + macID;
  int retries = 0;

  while (retries < MAX_RETRIES) {
    Serial.print("Sending HELLO with MAC (try ");
    Serial.print(retries + 1);
    Serial.println(")...");

    int txState = radio.transmit(message.c_str());
    if (txState != RADIOLIB_ERR_NONE) {
      Serial.print("Transmit failed, code ");
      Serial.println(txState);
      retries++;
      delay(1000);
      continue;
    }

    // Wait for HELLO_ACK
    uint8_t ackBuf[32] = {0};
    int ackLen = sizeof(ackBuf);
    unsigned long startTime = millis();

    while (millis() - startTime < ACK_TIMEOUT) {
      memset(ackBuf, 0, ackLen);
      int rxLen = ackLen;
      int rxState = radio.receive(ackBuf, ackLen);
      if (rxState == RADIOLIB_ERR_NONE) {
        ackBuf[ackLen] = 0;
        Serial.print("Received: ");
        Serial.println((char*)ackBuf);

        if (strcmp((char*)ackBuf, "HELLO_ACK") == 0) {
          Serial.println("Connection ACK received!");
          return true;
        }
      }
    }

    Serial.println("No ACK received, retrying...");
    retries++;
  }

  return false;
}


void loop() {
  bool x= establishConnection();
  Serial.print("sending to sleep\n");
  radio.sleep();
  delay(6000);
  radio.standby(RADIOLIB_SX126X_STANDBY_RC, true);
  Serial.println("device woke up");
  delay(2000);
}
