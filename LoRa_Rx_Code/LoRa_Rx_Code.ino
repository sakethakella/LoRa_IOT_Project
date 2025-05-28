#include <RadioLib.h>
#include <STM32RTC.h>
#include <U8g2lib.h>
#include <Wire.h>
#include <TimeLib.h>

STM32WLx radio = new STM32WLx_Module();
STM32RTC &rtc = STM32RTC::getInstance();
TwoWire myWire(PA9, PA10); // SCL, SDA for Wio-E5
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/U8X8_PIN_NONE, /* clock=*/PA9, /* data=*/PA10);


const byte setseconds = 0;
const byte setminutes = 25;
const byte sethours = 13;
const byte setweekDay = 3;
const byte setday = 28;
const byte setmonth = 5;
const byte setyear = 25;

time_t convertToUnixTime(int year, int month, int day, int hour, int minute, int second) {
  tmElements_t tm;
  tm.Year = year - 1970;  // TimeLib counts years since 1970
  tm.Month = month;
  tm.Day = day;
  tm.Hour = hour;
  tm.Minute = minute;
  tm.Second = second;
  return makeTime(tm);
}

void setup()
{
  Serial.begin(115200);
  rtc.begin();
  rtc.setHours(sethours);
  rtc.setMinutes(setminutes);
  rtc.setSeconds(setseconds);
  rtc.setWeekDay(setweekDay);
  rtc.setDay(setday);
  rtc.setMonth(setmonth);
  rtc.setYear(setyear);
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
  int state = radio.begin(868.0, 125.0, 12, 5, RADIOLIB_SX126X_SYNC_WORD_PRIVATE, 14, 8, 1.7, false);
  if (state != RADIOLIB_ERR_NONE)
  {
    Serial.print("LoRa init failed, code ");
    Serial.println(state);
    while (true)
      ;
  }
  Serial.println("LoRa Receiver ready.");
  u8g2.begin();
  u8g2.clearBuffer();          
  u8g2.setFont(u8g2_font_ncenB08_tr); 
  u8g2.drawStr(0, 24, "Hello Wio-E5!");  
  u8g2.sendBuffer();  
}

void loop()
{
  uint8_t buffer[17] = {0};
  int len = sizeof(buffer);
  int state = radio.receive(buffer, len);
  float rssi=radio.getRSSI();
  time_t unixTime = convertToUnixTime(2000 + rtc.getYear(), rtc.getMonth(), rtc.getDay(), rtc.getHours(), rtc.getMinutes(), rtc.getSeconds());
  if (state == RADIOLIB_ERR_NONE)
  {

    Serial.printf("%02d/%02d/%02d ", rtc.getDay(), rtc.getMonth(), rtc.getYear());
    Serial.printf("%02d:%02d:%02d.%03d\n", rtc.getHours(), rtc.getMinutes(), rtc.getSeconds(), rtc.getSubSeconds());
    Serial.printf("Unix Time: %lu\n", (unsigned long)unixTime);
    Serial.print(rssi);
    Serial.print("dBm \n");
    Serial.print("Received: ");
    for (int i = 0; i < len; i++) {
      if(buffer[i]<0x10)Serial.print("0");
      Serial.print(buffer[i],HEX);
      Serial.print(" ");
    }
    Serial.println();
    radio.transmit("HELLO_ACK");
    Serial.println("ACK Sent");
    
  }
  else if (state != RADIOLIB_ERR_RX_TIMEOUT)
  {
    Serial.print("Receive failed, code ");
    Serial.println(state);
  }
}
