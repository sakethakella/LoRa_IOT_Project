<h1>LoRa Communication with Seeed Studio LoRa-E5 &amp; STM32WLx Dev Kits</h1>

<p>This project demonstrates how to establish LoRa communication between many Seeed Studio LoRa-E5 boards (STM32WLx-based) in star topology using Arduino IDE. It includes a transmitter that sends custom temperature data along with unique device identifiers and a receiver that captures the data, logs it with timestamps, and acknowledges reception.</p>

<h3>Project Overview</h3>
<p>We worked with <strong>Seeed Studio LoRa-E5 development kits</strong> to build a reliable low-power wireless communication link over LoRa. The key aspects of our work:</p>
<ul>
  <li>Successfully programmed STM32WLx boards using <strong>Arduino IDE</strong>, bypassing the need for ST’s official programmer tools.</li>
  <li>Developed <strong>transmitter and receiver firmware</strong> to exchange custom data (temperature readings).</li>
  <li>Incorporated <strong>device unique IDs (MAC addresses)</strong> and real-time <strong>timestamps</strong> in transmitted messages.</li>
  <li>Implemented a <strong>handshake protocol</strong> where the receiver acknowledges received messages, ensuring communication reliability.</li>
</ul>

<h3>Hardware Used</h3>
<ul>
  <li>Seeed Studio <strong>LoRa-E5 Dev Kit</strong> (STM32WLx MCU with SX126x LoRa radio)</li>
  <li>ST-Link v2 or compatible SWD programmer (for initial board setup)</li>
  <li>USB cables and basic prototyping setup</li>
</ul>

<h3>Software Setup</h3>
<ol>
  <li><strong>Install STM32 Board in Arduino IDE:</strong>
    <ul>
      <li>Add the STM32 board URL in Arduino IDE preferences:<br>
          <code>https://github.com/stm32duino/BoardManagerFiles/raw/main/package_stm_index.json</code>
      </li>
      <li>Open Board Manager and install the <strong>STM32WL</strong> package.</li>
    </ul>
  </li>
  <li><strong>Configure Upload Mode:</strong>
    <ul>
      <li>Select <strong>STLink</strong> or <strong>STProgrammerSWD</strong> as the upload method in Arduino IDE.</li>
      <li>Connect the board via USB or ST-Link debugger for flashing.</li>
      <li>This allows uploading code directly via Arduino IDE, avoiding the ST official programmer software.</li>
    </ul>
  </li>
  <li><strong>Install RadioLib Library:</strong>
    <ul>
      <li>Use Arduino Library Manager to install <strong>RadioLib</strong> for LoRa communication support.</li>
    </ul>
  </li>
</ol>

<h3>Code Description</h3>
<h4>Transmitter</h4>
<ul>
  <li>Initializes the LoRa module at 868 MHz with configured spreading factor and bandwidth.</li>
  <li>Reads and sends a custom message containing:
    <ul>
      <li>Device unique ID (using STM32’s unique hardware ID)</li>
      <li>Custom temperature data (can be expanded to real sensor data)</li>
    </ul>
  </li>
  <li>Retries transmission up to 3 times if no acknowledgment is received.</li>
  <li>Waits for an <strong>ACK ("HELLO_ACK")</strong> response from the receiver to confirm successful delivery.</li>
</ul>

<h4>Receiver</h4>
<ul>
  <li>Initializes LoRa radio with matching parameters.</li>
  <li>Receives messages from the transmitter.</li>
  <li>Decodes incoming messages from hexadecimal format to ASCII.</li>
  <li>Logs received messages with real-time timestamp from the STM32 RTC.</li>
  <li>Sends back acknowledgment "HELLO_ACK" to the transmitter.</li>
</ul>

<h3>Usage</h3>
<p>Upload the <strong>transmitter</strong> code to one LoRa-E5 board and the <strong>receiver</strong> code to another. Open serial monitors on both boards to view transmitted and received data. Observe temperature data, device IDs, timestamps, and handshake acknowledgments for communication reliability.</p>

<h3>Areas where things can go wrong</h3>
<ul>
  <li>Not using proper stlink</li>
  <p>If not used proper ST-Link, the code may not upload and your device may not be found at all.</p>
  <li>Proper configuration in Arduino IDE</li>
  <p>Properly configure your device in Arduino IDE by correct selection of boards and settings.</p>
</ul>

<hr/>
