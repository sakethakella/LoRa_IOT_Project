#include <RadioLib.h>
#include <STM32RTC.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <Adafruit_SHT4x.h>
#include <time.h>

// DS18B20 setup
#define ONE_WIRE_BUS A3
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature ds18b20(&oneWire);

// SHT41 setup
Adafruit_SHT4x sht4 = Adafruit_SHT4x();

// LoRa radio
STM32WLx radio = new STM32WLx_Module();
STM32RTC &rtc = STM32RTC::getInstance();

// Initial RTC time
const byte setseconds = 0;
const byte setminutes = 25;
const byte sethours = 13;
const byte setweekDay = 3;
const byte setday = 28;
const byte setmonth = 5;
const byte setyear = 25;  // Only last two digits, e.g., 25 for 2025

// Convert to Unix timestamp
uint32_t convertToUnixTime(int year, int month, int day, int hour, int minute, int second) {
    struct tm t = {0};
    t.tm_year = year - 1900;
    t.tm_mon  = month - 1;
    t.tm_mday = day;
    t.tm_hour = hour;
    t.tm_min  = minute;
    t.tm_sec  = second;
    t.tm_isdst = 0;
    time_t epochTime = mktime(&t);
    return (uint32_t)epochTime;
}

void setup() {
  Serial.begin(115200);
  while (!Serial);

  // Initialize RTC
  rtc.begin();  // Just in case it hasn't been started
  rtc.setTime(sethours, setminutes, setseconds);
  rtc.setDate(setday, setmonth, setyear);
  rtc.setWeekDay(setweekDay);

  // Sensors
  ds18b20.begin();
  sht4.begin();

  // LoRa setup
  static const uint32_t rfswitch_pins[5] = {PA4, PA5, RADIOLIB_NC, RADIOLIB_NC, RADIOLIB_NC};
  static const Module::RfSwitchMode_t rfswitch_table[] = {
    {STM32WLx::MODE_IDLE,  {LOW,  LOW}},
    {STM32WLx::MODE_RX,    {HIGH, LOW}},
    {STM32WLx::MODE_TX_HP, {LOW,  HIGH}},
    END_OF_MODE_TABLE
  };

  radio.setRfSwitchTable(rfswitch_pins, rfswitch_table);
  int state = radio.begin(868.0, 125.0, 12, 5, RADIOLIB_SX126X_SYNC_WORD_PRIVATE, 14, 8, 1.7, false);
  if (state != RADIOLIB_ERR_NONE) {
    Serial.print("LoRa init failed, code ");
    Serial.println(state);
    while (true);
    
  }
  else Serial.println("Lora Transmitter Ready!");
}

void loop() {
  uint8_t payload[17];
  payload[0] = 0xAA;

  // Unique Device ID
  uint32_t uid0 = *(uint32_t*)0x1FFF7590;
  payload[1] = (uid0 >> 24) & 0xFF;
  payload[2] = (uid0 >> 16) & 0xFF;
  payload[3] = (uid0 >> 8) & 0xFF;
  payload[4] = uid0 & 0xFF;

  // DS18B20 temperature
  ds18b20.requestTemperatures();
  float tempC = ds18b20.getTempCByIndex(0);
  int16_t tempInt = (int16_t)(tempC * 100);
  payload[5] = (tempInt >> 8) & 0xFF;
  payload[6] = tempInt & 0xFF;

  // SHT41 humidity
  sensors_event_t humidity_event, temp_event;
  sht4.getEvent(&humidity_event, &temp_event);
  uint16_t humidityInt = (uint16_t)(humidity_event.relative_humidity * 100);
  payload[7] = (humidityInt >> 8) & 0xFF;
  payload[8] = humidityInt & 0xFF;

  // Battery (dummy)
  payload[9] = 95;

  // Timestamp
  int year = 2000 + rtc.getYear();
  uint32_t timestamp = convertToUnixTime(
    year,
    rtc.getMonth(),
    rtc.getDay(),
    rtc.getHours(),
    rtc.getMinutes(),
    rtc.getSeconds()
  );

  Serial.print("Unix Timestamp: ");
  Serial.println(timestamp);

  payload[10] = (timestamp >> 24) & 0xFF;
  payload[11] = (timestamp >> 16) & 0xFF;
  payload[12] = (timestamp >> 8) & 0xFF;
  payload[13] = timestamp & 0xFF;

  // CRC
  uint8_t crc = 0;
  for (int i = 0; i < 14; i++) {
    crc += payload[i];
  }
  payload[14] = crc;

  // Stop byte
  payload[15] = 0xFF;

  // Send payload
  int txState = radio.transmit(payload, 17);
  if (txState == RADIOLIB_ERR_NONE) {
    Serial.println("Payload sent!");
  } else {
    Serial.print("Send failed, code: ");
    Serial.println(txState);
  }

  // Sleep & wake cycle
  radio.sleep();
  Serial.println("Sleeping...");
  delay(15000);
  radio.standby(RADIOLIB_SX126X_STANDBY_RC, true);
  Serial.println("Woke up!");
  delay(2000);
}