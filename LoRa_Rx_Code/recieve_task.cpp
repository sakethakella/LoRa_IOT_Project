#include "recieve_task.h"

static void logHexBuf(const uint8_t *buf, size_t len) {
  for (size_t i = 0; i < len; ++i) {
    if (buf[i] < 0x10) Serial.print('0');
    Serial.print(buf[i], HEX);
    Serial.print(' ');
  }
}

static void logError(const char* prefix, int code) {
  Serial.print(prefix);
  Serial.println(code);
}

void ReceiveTask(void *pvParameters) {
  constexpr TickType_t SHORT_DELAY = pdMS_TO_TICKS(10);
  PacketMsg msg;

  for (;;) {
    uint8_t buffer[PACKET_BUFFER_LEN];
    int state = radio.receive(buffer, sizeof(buffer));

    if (state == RADIOLIB_ERR_NONE) {
      float rssi = radio.getRSSI();

      if (buffer[0] == 0xAA && buffer[16] == 0xFF && crccheck(buffer)) {
        Serial.println("Valid data packet received!");

        uint8_t ack[ACK_PACKET_LEN];
        send_ack(buffer, ack, rssi);

        int txState = radio.transmit(ack, sizeof(ack));
        if (txState == RADIOLIB_ERR_NONE) {
          Serial.println("ACK sent successfully!");
          memcpy(msg.buffer, buffer, PACKET_BUFFER_LEN);
          msg.rssi = rssi;
          xQueueSend(packetQueue, &msg, SHORT_DELAY);
        }
        else {
          logError("Failed to send ACK, code: ", txState);
        }
      }
      else {
        Serial.println("Invalid packet received (header/footer/CRC).");
      }
    }
    else if (state != RADIOLIB_ERR_RX_TIMEOUT && state != RADIOLIB_ERR_CRC_MISMATCH) {
      logError("LoRa receive failed, code: ", state);
    }

    vTaskDelay(SHORT_DELAY);
  }
}

void PrintTask(void *pvParameters) {
  while (!Serial) {
    vTaskDelay(pdMS_TO_TICKS(10));
  }

  PacketMsg msg;
  for (;;) {
    if (xQueueReceive(packetQueue, &msg, portMAX_DELAY) == pdTRUE) {
      logHexBuf(msg.buffer, PACKET_BUFFER_LEN);
      Serial.println();
    }
  }
}

void WatchdogTask(void *pvParameters) {
  constexpr TickType_t KICK_DELAY = pdMS_TO_TICKS(500);
  for (;;) {
    IWDG->KR = 0xAAAA;
    vTaskDelay(KICK_DELAY);
  }
}
