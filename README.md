# Internet of Things Coursework Portfolio

Selected projects completed for the **Internet of Things** course at Politecnico di
Milano during the 2024/2025 academic year.

The work spans embedded sensing, low-power wireless communication, packet analysis,
Node-RED automation, LoRaWAN capacity planning, and end-to-end IoT architecture.
This repository keeps the implementation, reports, selected measurements, and useful
figures while excluding grading files, build caches, duplicate archives, raw packet
captures, and instructor-provided material.

## Projects

### 1. Low-power ESP32 parking sensor

An ESP32 and HC-SR04 ultrasonic sensor are used to detect parking-space occupancy.
The node broadcasts `FREE` or `OCCUPIED` through ESP-NOW, disables Wi-Fi after
transmission, and enters deep sleep between measurements.

<p align="center">
  <img src="challenge_1/figures/After_OPT_Energy_consumption_Except_DeepSleep.png"
       width="760"
       alt="Measured energy consumption by ESP32 operating state after optimization">
</p>

Key material:

- Firmware: [`challenge_1/submission/esp32IoT_Challenge01_CHEN_Hong`](challenge_1/submission/esp32IoT_Challenge01_CHEN_Hong/)
- Power measurements: [`challenge_1`](challenge_1/)
- Figures: [`challenge_1/figures`](challenge_1/figures/)
- Implementation report: [`IoT_Challenge_01 (3).pdf`](challenge_1/submission/IoT_Challenge_01%20(3).pdf)
- Sink-position optimization: [`IoT_Challenge01_Exercise.pdf`](challenge_1/submission/IoT_Challenge01_Exercise.pdf)

The study compares state-level time and energy consumption before and after shortening
the active Wi-Fi period.

### 2. CoAP, MQTT, and MQTT-SN packet analysis

Packet traces are filtered to investigate CoAP requests, MQTT subscriptions, retained
messages, last wills, QoS settings, and MQTT-SN traffic. A second study compares the
communication energy of CoAP and MQTT over a 24-hour sensor/valve scenario.

<p align="center">
  <img src="challenge_2/figures/CQ6.png"
       width="760"
       alt="Wireshark analysis of retained MQTT QoS 0 publish messages">
</p>

Key material:

- Analysis scripts: [`challenge_2/submission/shell`](challenge_2/submission/shell/)
- Packet-analysis report: [`Challenge.pdf`](challenge_2/submission/Challenge.pdf)
- Protocol-energy report: [`Exercise.pdf`](challenge_2/submission/Exercise.pdf)
- Supporting figures: [`challenge_2/figures`](challenge_2/figures/)

Selected results for the stated scenario:

- Most efficient CoAP configuration: **137.84 mJ**
- Most efficient MQTT configuration: **132.26 mJ**
- MQTT optimization proposals: **122.08 mJ** and **5.84 mJ**

### 3. Node-RED processing and LoRaWAN capacity

The Node-RED flow generates identifiers, publishes and subscribes through a local
Mosquitto broker, processes packet-derived records, rate-limits outgoing messages,
extracts temperature readings, counts acknowledgements, and exports a metric to
ThingSpeak.

The LoRaWAN part evaluates airtime, spreading-factor selection, success probability,
and capacity-improvement strategies. In the analytical scenario, **SF8** was the
highest spreading factor satisfying the required 70% success target, with an estimated
success rate of **73.5%**.

<p align="center">
  <img src="challenge_3/figures/Flow_tot.png"
       width="760"
       alt="Node-RED flow for MQTT filtering, rate limiting, visualization, and acknowledgement logging">
</p>

Key material:

- Sanitized Node-RED flow: [`flows.example.json`](challenge_3/submission/Part1_Challenge03/flows.example.json)
- Node-RED report: [`Challenge.pdf`](challenge_3/submission/Part1_Challenge03/Challenge.pdf)
- LoRaWAN notebooks and report: [`Part2_Exercise`](challenge_3/submission/Part2_Exercise/)
- Supporting figures: [`challenge_3/figures`](challenge_3/figures/)

Before importing the flow, replace `YOUR_THINGSPEAK_WRITE_API_KEY` in a local copy.
Never commit a real write key.

### 4. IoT system design and MAC-layer analysis

The final homework contains three system-level studies:

1. A low-cost forklift tracking architecture combining GPS/IMU, BLE-assisted indoor
   localization, LoRa, MQTT, Node-RED processing, storage, and dashboards.
2. IEEE 802.15.4 beacon-enabled CFP sizing for camera nodes with Poisson-distributed
   output sizes; the initial design obtains a **6.08% duty cycle**.
3. Dynamic-Frame ALOHA analysis for four RFID tags; among initial frame sizes 1–6,
   `r1 = 4` gives the highest reported efficiency, approximately **0.453**.

<p align="center">
  <img src="Homework/Exercise1/figures/block_new.png"
       width="760"
       alt="End-to-end forklift sensing, LoRa, MQTT, processing, storage, and visualization architecture">
</p>

Reports are under [`Homework/reports`](Homework/reports/).

## Reproducing the work

### ESP32 firmware

Open the PlatformIO project under
`challenge_1/submission/esp32IoT_Challenge01_CHEN_Hong/`. The same
directory includes Wokwi configuration and the simulated circuit definition.

### Node-RED

Import `flows.example.json`, configure a local Mosquitto broker on the port described
in the report, and add your own ThingSpeak write key only in the local Node-RED
credential/configuration.

### LoRaWAN notebooks

Open the notebooks in
`challenge_3/submission/Part2_Exercise/` and install the libraries imported by
the notebooks. The original course dataset and reference paper are intentionally not
redistributed by the repository.

## Authorship

All technical work in this repository—including system design, implementation,
experiments, analysis, figures, notebooks, and report preparation—was completed
independently by **Hong Chen**.

Some original submission PDFs retain additional names because the coursework was
submitted under a team-based administrative format. Those names reflect the submission
grouping rather than contributions to the work presented here.

## Academic integrity

These materials document completed coursework. Current students should follow their
institution's academic-integrity rules and must not submit this work as their own.
