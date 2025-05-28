#include <RadioLib.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <Wire.h> //SHT41X
#include <Adafruit_SHT4x.h>// SHT41

<<<<<<< HEAD
#define VBAT_PIN PA_1  // Change if needed
#define ADC_MAX 4095.0
#define VREF 3.3       // 3.3V ADC reference

// Voltage divider ratio (if VBAT goes through a divider like 1/2)
#define VOLTAGE_DIVIDER_RATIO 2.0

=======
>>>>>>> parent of 9ed71da (Updated_Lora_Tx)
// DS18B20 connected to pin PA0 (change as needed)
#define ONE_WIRE_BUS A3
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

// SHT41 (I2C)
Adafruit_SHT4x sht4 = Adafruit_SHT4x();

// Create the STM32WLx radio object
STM32WLx radio = new STM32WLx_Module();
const int MAX_RETRIES = 3;
const unsigned long ACK_TIMEOUT = 4000;  // 10 seconds

String getUniqueID() {
  uint32_t uid0 = *(uint32_t*)0x1FFF7590;
  char buffer[9];  // 8 hex digits + null terminator
  sprintf(buffer, "%08lX", uid0);  // Capital hex, zero-padded
  return String(buffer);
<<<<<<< HEAD
}

float readBatteryVoltage() {
  uint16_t raw = analogRead(VBAT_PIN);
  float voltage = (raw / ADC_MAX) * VREF * VOLTAGE_DIVIDER_RATIO;
  return voltage;
}

// Convert voltage (e.g., 3.0–4.2V) to % assuming Li-ion
uint8_t batteryPercent(float voltage) {
  if (voltage >= 4.2) return 100;
  if (voltage <= 3.0) return 0;
  return (uint8_t)((voltage - 3.0) / (4.2 - 3.0) * 100.0);
=======
>>>>>>> parent of 9ed71da (Updated_Lora_Tx)
}


void setup() {
  Serial.begin(115200);
  while (!Serial);
 // Initialize DS18B20
  sensors.begin();
<<<<<<< HEAD
  analogReadResolution(12);  // 12-bit ADC
  analogReference(AR_DEFAULT);  // Usually 3.3V
=======
>>>>>>> parent of 9ed71da (Updated_Lora_Tx)

   // Initialize SHT41
  if (!sht4.begin()) {
    Serial.println("Couldn't find SHT4x sensor!");
  } else {
    Serial.println("SHT4x sensor initialized");
    sht4.setPrecision(SHT4X_HIGH_PRECISION);
    sht4.setHeater(SHT4X_NO_HEATER);
  }

  // Configure RF switch for LoRa-E5
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
  int state = radio.begin(868.0, 125.0, 12, 5, RADIOLIB_SX126X_SYNC_WORD_PRIVATE, 14, 8, 1.7, false);
  if (state != RADIOLIB_ERR_NONE) {
    Serial.print("LoRa init failed, code ");
    Serial.println(state);
    while (true);
  }

  Serial.println("LoRa Transmitter Initialized - 868 MHz, SF6, BW125kHz, CR4/5");
  initialized = true;
}

float readTemperature() {
  sensors.requestTemperatures();
  return sensors.getTempCByIndex(0);  // Get temperature from first sensor
}


bool readSHT41(float &temp, float &humidity) {
  sensors_event_t temp_event, humidity_event;

  sht4.getEvent(&humidity_event, &temp_event);  // blocks for measurement
  temp = temp_event.temperature;
  humidity = humidity_event.relative_humidity;
  
  return !isnan(temp) && !isnan(humidity);  // true if valid
}
  
bool establishConnection() {
  initializeRadio();
  String macID = getUniqueID();
<<<<<<< HEAD
  float voltage = readBatteryVoltage();
  uint8_t battery = batteryPercent(voltage);
  String message = "Battery: " + String(voltage, 2) + "V (" + String(battery) + "%)";
=======
>>>>>>> parent of 9ed71da (Updated_Lora_Tx)
  float temp = readTemperature();
  float shtTemp = 0.0, shtHum = 0.0;
  bool shtOK = readSHT41(shtTemp, shtHum);
  String message = "DS18B20: " + String(temp, 2) + " C";
  if (shtOK) {
    message += ", SHT41 Temp: " + String(shtTemp, 2) + " C, Hum: " + String(shtHum, 2) + " %";
  } else {
    message += ", SHT41: N/A";
  }
  message += ", ID: \"" + macID + "\"";

  Serial.println("Sending message: " + message);

  int retries = 0;
  while (retries < MAX_RETRIES) {
    int txState = radio.transmit(message.c_str());
    if (txState != RADIOLIB_ERR_NONE) {
      Serial.print("Transmit failed, code ");
      Serial.println(txState);
      retries++;
      delay(1000);
      continue;
    }

    uint8_t ackBuf[128] = {0};
    int ackLen = sizeof(ackBuf);
    unsigned long startTime = millis();

    while (millis() - startTime < ACK_TIMEOUT) {
      memset(ackBuf, 0, ackLen);
      int rxState = radio.receive(ackBuf, ackLen);
      if (rxState == RADIOLIB_ERR_NONE) {
        ackBuf[ackLen] = 0;
        Serial.print("Received: ");
        Serial.println((char*)ackBuf);
        if (strcmp((char*)ackBuf, "HELLO_ACK") == 0) {
          Serial.println("ACK received!");
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
  delay(10000);
  radio.standby(RADIOLIB_SX126X_STANDBY_RC, true);
  Serial.println("device woke up");
  delay(2000);
}
