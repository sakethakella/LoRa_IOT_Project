<h1 align="center">📡 LoRa Receiver &amp; Transmitter Nodes with FreeRTOS &amp; Linear Feedback Controller</h1>
<p align="center">
  <img src="https://img.shields.io/badge/Platform-STM32WLx-blue" alt="Platform: STM32WLx" />
  <img src="https://img.shields.io/badge/LoRa-RadioLib-orange" alt="LoRa: RadioLib" />
  <img src="https://img.shields.io/badge/RTOS-FreeRTOS-green" alt="RTOS: FreeRTOS" />
  <img src="https://img.shields.io/badge/Watchdog-Enabled-critical" alt="Watchdog: Enabled" />
  <img src="https://img.shields.io/badge/Control-LinearFeedback-purple" alt="Control: Linear Feedback" />
</p>

<h2>📝 About the Project</h2>
<p>This repository provides a production-ready implementation of a bidirectional LoRa communication system optimized for low-power applications. Two nodes collaborate seamlessly:</p>
<ul>
  <li><strong>Receiver Node</strong>: Based on STM32WLx LoRa-E5, uses FreeRTOS to orchestrate packet reception, data validation, logging, and ACK responses with minimal latency.</li>
  <li><strong>Transmitter Node</strong>: A battery-powered sensor unit that wakes periodically, collects temperature, humidity, battery level, and timestamp, then transmits and adapts its TX power via a linear feedback controller.</li>
</ul>
<p><strong>Ideal For</strong>: Remote sensing in agriculture, environmental monitoring, asset tracking, smart metering, and any IoT scenario demanding reliable two-way links under tight energy constraints.</p>

<h2>🔍 Detailed Architecture</h2>
<p><code>Receiver</code> ⇄ <code>LoRa RF Link</code> ⇄ <code>Transmitter</code></p>
<p>The design separates concerns across layers for maintainability and extensibility.</p>

<h3>Receiver Node Layers</h3>
<ol>
  <li><strong>Transport (RadioLib)</strong>: Encapsulates SX126x driver setup (868 MHz, SF7, BW125, CR4/5).</li>
  <li><strong>RTOS Scheduler</strong>: Launches three tasks—<code>ReceiveTask</code> (high), <code>PrintTask</code> (medium), <code>WatchdogTask</code> (low).</li>
  <li><strong>Validation</strong>: Checks packet structure [0xAA…0xFF], Checksum integrity, and expected UID.</li>
  <li><strong>ACK Generation</strong>: Builds 7-byte response: start byte, 4‑byte UID echo, 2‑byte RSSI.</li>
</ol>
<p>Tasks communicate via FreeRTOS queues, ensuring non-blocking operations and robust error isolation.</p>

<h3>Transmitter Node Layers</h3>
<ol>
  <li><strong>Sensing</strong>: Adafruit SHT4x over I2C for temperature/humidity; ADC with VREFINT calibration for battery voltage.</li>
  <li><strong>Timekeeping</strong>: STM32RTC provides calibrated timestamps embedded in payload.</li>
  <li><strong>Linear Feedback Controller</strong>: Adjusts TX power each cycle based on last ACK RSSI error to achieve target link margin.</li>
  <li><strong>Power Management</strong>:
    <ul>
      <li>Graceful wakeup reinitializes peripherals.</li>
      <li>STM32LowPower deep-sleep for configurable intervals (<2 µA standby).</li>
    </ul>
  </li>
  <li><strong>Resilience</strong>: Retries up to <code>MAX_RETRIES</code>, handles NACK/timeouts gracefully, and logs all events.</li>
</ol>

<h2>🔄 Node Workflows</h2>
<pre><code>Receiver:
1. Wait for LoRa packet
2. Validate header, UID, CRC
3. Log data (UART queue)
4. Send ACK with RSSI feedback
5. Repeat

Transmitter:

1. Wakeup & initialize
2. Read sensors + timestamp
3. Transmit packet
4. Wait for ACK (timeout)
5. If ACK: adjust TX power; else retry
6. Deep sleep
7. Repeat
   </code></pre>

<h2>🔑 Key Features &amp; Configurations</h2>
<table border="1" cellspacing="0" cellpadding="8">
  <tr><th>Feature</th><th>Receiver</th><th>Transmitter</th></tr>
  <tr><td>RTOS</td><td>FreeRTOS (3 tasks)</td><td>—</td></tr>
  <tr><td>Packet Integrity</td><td>Checksum + header/footer</td><td>Checksum on raw payload</td></tr>
  <tr><td>ACK Protocol</td><td>7-byte response (UID &amp; RSSI)</td><td>Processes RSSI feedback</td></tr>
  <tr><td>TX Control</td><td>—</td><td>Linear feedback algorithm</td></tr>
  <tr><td>Deep Sleep</td><td>—</td><td>&lt;2 µA idle draw</td></tr>
  <tr><td>Sensors</td><td>—</td><td>Temperature, Humidity, Vbat</td></tr>
  <tr><td>Timestamp</td><td>—</td><td>RTC (Unix epoch)</td></tr>
  <tr><td>Retries</td><td>—</td><td>Configurable attempts &amp; timeout</td></tr>
</table>
<p><em>Configure <code>DESIRED_RSSI_DBM</code>, <code>MAX_RETRIES</code>, and sleep duration in <code>transmitter.ino</code>.</em></p>

<h2>📦 Packet &amp; Payload Formats</h2>
<h3>Sensor Packet (17 bytes)</h3>
<pre><code>[0xAA] [UID(4B)] [Temp×100:2B] [Hum×10:2B] [Vbat:2B] [Timestamp:4B] [checksum:1B] [0xFF]</code></pre>
<ul>
  <li><strong>Temp</strong>: signed int16 in centi‑°C.</li>
  <li><strong>Hum</strong>: uint16 in deci‑% RH.</li>
  <li><strong>Vbat</strong>: uint16 in mV.</li>
  <li><strong>Timestamp</strong>: uint32 Unix time.</li>
</ul>
<h3>ACK Packet (7 bytes)</h3>
<pre><code>[0x55] [UID(4B)] [RSSI×10:2B]</code></pre>
<p><strong>RSSI</strong>: signed int16 representing dBm×10 upto 2 decimal precision.</p>

<h2>🧩 Component Deep Dive</h2>
<h3>STM32WLx (LoRa-E5)</h3>
<ul>
  <li>Integrated SX126x transceiver, dual‑core M4 architecture.</li>
  <li>Robust low-power modes and power rail monitoring.</li>
</ul>
<h3>RadioLib &amp; SX126x</h3>
<ul>
  <li>Supports private sync word, CRC, and LoRa parameters.</li>
  <li>High-level API simplifies RX/TX switching.</li>
</ul>
<h3>Adafruit SHT4x</h3>
<ul>
  <li>High‑precision temperature and humidity sensor.</li>
  <li>Low-power standby between measurements.</li>
</ul>
<h3>STM32RTC &amp; LowPower API</h3>
<ul>
  <li>Calendar, alarm, and backup registers for reliable wakeups.</li>
  <li>Deep-sleep current <2 µA with minimal wake overhead.</li>
</ul>

<h2>🚀 Setup &amp; Deployment</h2>
<ol>
  <li><strong>Clone the Repo</strong>:
    <pre><code>git clone https://github.com/ideabytes/LORA_Project.git</code></pre>
  </li>
  <li><strong>Install Arduino Core &amp; Libraries</strong>: STM32duino via Board Manager; libraries: STM32FreeRTOS, RadioLib, STM32RTC, Adafruit_SHT4x, STM32LowPower.</li>
  <li><strong>Customize Settings</strong> in code:
    <pre><code>#define DESIRED_RSSI_DBM -90.0
#define MAX_RETRIES       3
#define ACK_TIMEOUT_MS    200
#define SLEEP_DURATION_MS 60000</code></pre>
  </li>
  <li><strong>Upload Sketches</strong>: Load <code>receiver.ino</code> and <code>transmitter.ino</code> in Arduino IDE to respective boards.</li>
  <li><strong>Monitor Logs</strong>: Use Serial Monitor at 115200 baud; adjust <code>LOG_LEVEL</code> as needed.</li>
</ol>

<h2>📈 Examples &amp; Metrics</h2>
<p><strong>Sample Logs</strong>:</p>
<pre><code>[Receiver] Packet UID=0x1A2B3C4D; Temp=23.45°C; Hum=51.2%; Vbat=3280mV; RSSI=-82.7dBm → ACK sent
[Transmitter] ACK received; RSSI=-82.7dBm; TX power adjusted 20dBm→18dBm; entering sleep
</code></pre>
<ul>
  <li><strong>TX Current</strong> ~50 mA @22dBm</li>
  <li><strong>Deep-Sleep Current</strong> &lt;2 µA</li>
  <li><strong>ACK Latency</strong> &lt;15 ms</li>
</ul>

<h2>👨‍💻 Contributors</h2>
<p>
  <strong>Akella Satya Hanuma Saketh</strong><br />
  📧 <a href="mailto:sakethakella05@gmail.com">Saketh Akella</a><br />
  🔗 <a href="https://www.linkedin.com/in/saketh-akella-aa7886270/" target="_blank">LinkedIn</a>
</p>
<p>
  <strong>Thirumal Reddy Bandi</strong><br />
  📧 <a href="mailto:Bandi Thirumal Reddy">thirumal Bandi</a><br />
  🔗 <a href="https://www.linkedin.com/in/thirumal-reddy-bandi-34a7a3250/" target="_blank">LinkedIn</a>
</p>
