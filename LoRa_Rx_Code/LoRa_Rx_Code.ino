#include <RadioLib.h>
#include <STM32RTC.h>

STM32WLx radio = new STM32WLx_Module();
STM32RTC &rtc = STM32RTC::getInstance();

const byte seconds = 0;
const byte minutes = 0;
const byte hours = 12;

/* Change these values to set the current initial date */
/* Monday 15th June 2015 */
const byte weekDay = 2;
const byte day = 27;
const byte month = 5;
const byte year = 25;

void setup()
{
  Serial.begin(115200);
  rtc.begin();
  rtc.setHours(hours);
  rtc.setMinutes(minutes);
  rtc.setSeconds(seconds);
  rtc.setWeekDay(weekDay);
  rtc.setDay(day);
  rtc.setMonth(month);
  rtc.setYear(year);
  while (!Serial)
    ;
  static const uint32_t rfswitch_pins[5] = {PA4, PA5, RADIOLIB_NC, RADIOLIB_NC, RADIOLIB_NC};
  static const Module::RfSwitchMode_t rfswitch_table[] = {

      {STM32WLx::MODE_IDLE, {LOW, LOW}},

      {STM32WLx::MODE_RX, {HIGH, LOW}},

      {STM32WLx::MODE_TX_HP, {LOW, HIGH}},

      END_OF_MODE_TABLE

  };

  radio.setRfSwitchTable(rfswitch_pins, rfswitch_table);
  int state = radio.begin(868.0, 125.0, 9, 5, RADIOLIB_SX126X_SYNC_WORD_PRIVATE, 14, 8, 1.7, false);
  if (state != RADIOLIB_ERR_NONE)
  {
    Serial.print("LoRa init failed, code ");
    Serial.println(state);
    while (true)
      ;
  }
  Serial.println("LoRa Receiver ready.");
}
String hexToAscii(const String &hex)
{
  String ascii = "";
  for (size_t i = 0; i < hex.length(); i += 2)
  {
    String byteStr = hex.substring(i, i + 2);
    char c = strtol(byteStr.c_str(), nullptr, 16);
    ascii += c;
  }
  return ascii;
}

// Example on receiver:

void handleReceivedMessage(char *rawHex)
{
  String hexMsg = String(rawHex);
  String decodedMsg = hexToAscii(hexMsg);
  Serial.print("Decoded Message: ");
  Serial.println(decodedMsg);
}

void loop()
{
  uint8_t buf[64] = {0};
  int len = sizeof(buf);
  int state = radio.receive(buf, len);
  float rssi=radio.getRSSI();

  if (state == RADIOLIB_ERR_NONE)
  {
    buf[len] = 0; // null-terminate

    Serial.printf("%02d/%02d/%02d ", rtc.getDay(), rtc.getMonth(), rtc.getYear());
    Serial.printf("%02d:%02d:%02d.%03d\n", rtc.getHours(), rtc.getMinutes(), rtc.getSeconds(), rtc.getSubSeconds());
    Serial.print(rssi);
    Serial.print("dBm \n");
    Serial.print("Received: ");
    Serial.println((char *)buf);
    radio.transmit("HELLO_ACK");
    Serial.println("ACK Sent");
  }
  else if (state != RADIOLIB_ERR_RX_TIMEOUT)
  {
    Serial.print("Receive failed, code ");
    Serial.println(state);
  }
}
