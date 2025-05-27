# LoRa-E5 Communication Project

## Overview

This project demonstrates successful communication between Seeed Studio LoRa-E5 boards using custom transmitter and receiver code. The communication involves sending temperature data with additional metadata such as MAC address and timestamp, leveraging the Arduino IDE for code development and uploading.

---

## Hardware Used

- [Seeed Studio LoRa-E5 Development Kit](https://wiki.seeedstudio.com/LoRa-E5_Dev_Board/)
- ST-LINK V2 or compatible programmer/debugger

---

## Software Requirements

- **Arduino IDE**
- STM32 board package installed via Arduino Board Manager
- ST-LINK drivers
- [STM32CubeProgrammer](https://www.st.com/en/development-tools/stm32cubeprog.html) (used for validation, not mandatory for upload)

---

## Setup Instructions

### 1. Arduino IDE Configuration

1. **Install STM32 Board Package:**
   - Go to `File` > `Preferences`
   - Add the following URL to *Additional Board Manager URLs*:  
     `https://github.com/stm32duino/BoardManagerFiles/raw/main/package_stmicroelectronics_index.json`
   - Open *Boards Manager*, search for “STM32” and install the package by STMicroelectronics.

2. **Board Selection:**
   - Select your LoRa-E5 board model from `Tools > Board > STM32 Boards`.

3. **Upload Settings:**
   - **Upload Method:** `STM32CubeProgrammer (SWD)`
   - **Port:** Select your ST-LINK port
   - **Programmer:** ST-LINK V2

4. **ST-LINK Connection:**
   - Connect ST-LINK to the LoRa-E5 board using SWD pins.
   - Ensure power is provided to the board.

5. **Upload via Arduino IDE:**
   - Code can now be uploaded directly using Arduino IDE. STM32CubeProgrammer is not required for flashing if set up correctly.

---

## Project Functionality

### Transmitter

The transmitter collects or generates temperature data and sends it over LoRa along with:

- MAC address
- Timestamp
- Optional custom message

### Receiver

The receiver listens for LoRa packets and extracts:

- Temperature value
- Sender MAC address
- Timestamp
- Other optional metadata

The received data can be logged or displayed via serial monitor for analysis and debugging.

---

## Code Structure

transmitter code
``

receiver code
``
