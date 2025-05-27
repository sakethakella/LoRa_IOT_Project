# 📡 Point-to-Point LoRa Communication using Seeed Wio-E5 (STM32WLE5) and RadioLib

This project demonstrates how to implement **raw LoRa communication (not LoRaWAN)** between two **Seeed Studio Wio-E5 Dev Kits** using the [RadioLib](https://github.com/jgromes/RadioLib) Arduino library.

> 🚫 No LoRaWAN server or gateway is used — this is a pure many to one LoRa setup.

---

## 🚀 Features

- LoRa radio setup using the onboard STM32WLE5JC module
- Transmits and receives **custom data messages** via LoRa
- Based on Arduino IDE + RadioLib for rapid development
- Works at **868 MHz** with **SF6**, **BW 500 kHz**, and **CR 4/5**

---

## 🧰 Hardware Requirements

- Seeed Studio Wio-E5 Dev Kit/mini
- USB-C cables
- Antennas (868 MHz recommended)
- ST-Link V2 (optional, for programming via STM32CubeProgrammer)

---

## 🖥️ Software Setup

### 1. Arduino IDE

- Install latest [Arduino IDE](https://www.arduino.cc/en/software)
- Add STM32 board support:
  - Go to **File > Preferences** > Additional Boards URL:
    ```
    https://github.com/stm32duino/BoardManagerFiles/raw/main/package_stmicroelectronics_index.json
    ```
  - Install **STM32 MCU based boards** in Boards Manager

- Install `RadioLib` library:
  - Go to **Sketch > Include Library > Manage Libraries**
  - Search and install **RadioLib by Jan Gromeš**

### 2. Select Board

- Board: `LoRa Boards`
- Variant: `LoRa E5 mini`
- Upload method: `STM32cubeprogrammer(swd)`

---

