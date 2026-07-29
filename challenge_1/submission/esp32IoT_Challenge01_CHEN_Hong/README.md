# ESP32 Parking-Occupancy Sensor

PlatformIO/Wokwi implementation of a low-power parking sensor using an ESP32, an HC-SR04 ultrasonic sensor, and ESP-NOW.

The firmware measures distance, classifies the space with a 50 cm threshold, broadcasts occupancy and distance to a sink, disables Wi-Fi after transmission, estimates state-level energy use, and enters timer-based deep sleep.

## Project Files

- `src/IoT_challenge_01_CHEN_Hong.ino` — firmware and energy/timing instrumentation
- `platformio.ini` — ESP32 Arduino build configuration
- `diagram.json` — Wokwi circuit
- `wokwi.toml` — Wokwi firmware/ELF paths

## Build

Install PlatformIO, open this directory, and run:

```bash
pio run
```

## Simulate with Wokwi

1. Install the Wokwi extension for VS Code.
2. Build the project once so `.pio/build/esp32/firmware.elf` and `firmware.bin` exist.
3. Open the Command Palette and run **Wokwi: Start Simulator**.

## Hardware and Configuration Notes

- HC-SR04 trigger pin: GPIO 5
- HC-SR04 echo pin: GPIO 18
- Occupied/free LEDs: GPIO 2 and GPIO 4
- Occupancy threshold: 50 cm
- Timer-based deep-sleep interval: 41 seconds
- ESP-NOW broadcast address, power assumptions, and battery-energy assumptions are defined in the source.

The power values and timing-derived energy calculations are coursework assumptions. Recalibrate them before using the firmware with physical hardware.
