#include <RadioLib.h>
#include <STM32RTC.h>
#include <U8g2lib.h>
#include <Wire.h>
#include <TimeLib.h>
#include <ArduinoJson.h>

//module instanciation
STM32WLx radio = new STM32WLx_Module();
STM32RTC &rtc = STM32RTC::getInstance();
TwoWire myWire(PA9, PA10);
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0,U8X8_PIN_NONE, PA9, PA10);

//globalvariable declaration
const byte setseconds = 0;
const byte setminutes = 25;
const byte sethours = 13;
const byte setweekDay = 3;
const byte setday = 28;
const byte setmonth = 5;
const byte setyear = 25;
float temperature;
float humidity;
int time11;
int battery;
float rssi;
int macaddress;

//crc check function
bool crccheck(uint8_t *buf){
   uint8_t crc=0;
   for(int i=0;i<14;i++){
      crc=crc+buf[i];
   }
   Serial.println(crc,HEX);
   if(crc==buf[14]){
    Serial.println("true");
    return true;
   }
   else return false;
}

//unix time converision file
time_t convertToUnixTime(int year, int month, int day, int hour, int minute, int second) {
  tmElements_t tm;
  tm.Year = year - 1970; 
  tm.Month = month;
  tm.Day = day;
  tm.Hour = hour;
  tm.Minute = minute;
  tm.Second = second;
  return makeTime(tm);
}


void getdata(uint8_t *buf){
    uint32_t macid=0;
    for (int i = 1; i <= 4; i++) {
    macid = (macid << 8) | buf[i];
    }
    macaddress=macid;
    int16_t temp=0;
    for(int i=5;i<=6;i++){
      temp= (temp<<8)|buf[i];
    }
    temperature=temp;
    uint16_t hum=0;
    for(int i=7;i<=8;i++){
      hum=(hum<<8)|buf[i];
    }
    humidity=hum;
    uint8_t bat=buf[9];
    battery=bat;
    uint32_t timestamp1=0;
    for(int i=10;i<=13;i++){
      timestamp1=(timestamp1<<8)|buf[i];
    }
    time11=timestamp1;

}

//create json file
void createJson() {
  StaticJsonDocument<128> doc;
  doc["macid"] = macaddress;
  doc["humidity"] = float(humidity/10);
  doc["temperature"] = float(temperature/100);
  doc["rssi"] = rssi;
  doc["battery"]=battery;
  doc["time11"]=time11;
  Serial.println();
  serializeJson(doc, Serial);
  Serial.println(); 
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
  while (!Serial);
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


void loop()
{
  uint8_t buffer[16] = {0};
  int len = sizeof(buffer);
  int state = radio.receive(buffer, len);
  rssi=radio.getRSSI();
  time_t unixTime = convertToUnixTime(2000 + rtc.getYear(), rtc.getMonth(), rtc.getDay(), rtc.getHours(), rtc.getMinutes(), rtc.getSeconds());
  
  if (state == RADIOLIB_ERR_NONE)
  {
    bool x=crccheck(buffer);
    if((buffer[0]==0xAA)&&(buffer[15]==0xFF)&&(x)){
    Serial.printf("Unix Time: %lu\n", (unsigned long)unixTime);
    Serial.print("Received: ");
    for (int i = 0; i < len; i++) {
      if(buffer[i]<0x10)Serial.print("0");
      Serial.print(buffer[i],HEX);
    }
    getdata(buffer);
    createJson();
    Serial.println();
    radio.transmit("HELLO_ACK");
    Serial.println("ACK Sent");
    }
    else {
      Serial.printf("start and stop bit dont match");
    }
  }
  else if (state != RADIOLIB_ERR_RX_TIMEOUT)
  {
    Serial.print("Receive failed, code ");
    Serial.println(state);
  }
}
