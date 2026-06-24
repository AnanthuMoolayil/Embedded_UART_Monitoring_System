# PIC16F877A Firmware

## Overview

Firmware for the **PIC16F877A** implementing UART-based monitoring, ADC acquisition, EEPROM persistence, uptime tracking, state monitoring, and event logging.

The firmware communicates with a host PC over UART and provides real-time system information, configuration management, and persistent storage of critical settings.

---

## Features

* Voltage & temperature monitoring via ADC
* EEPROM-backed persistent configuration for configurable temperature threshold
* Circular-buffer event logging
* Uptime tracking using Timer1
* UART command protocol
* System state monitoring (OK / OVERHEAT)

---

## Architecture

```text
+------------------------------------------------+
|                    Firmware                    |
+------------------------------------------------+
                     |
                     v
+-----------------------------------------------+
|               Command Processor               |
+-----------------------------------------------+
                     |
                     v
+-----------------------------------------------+
|                  Services                     |
|-----------------------------------------------|
| Sensor Service | Logging Service | Uptime     |
+-----------------------------------------------+
                     |
                     v
+-----------------------------------------------+
|                   Drivers                     |
|-----------------------------------------------|
| UART | ADC | EEPROM | Timer                   |
+-----------------------------------------------+
```

---

## EEPROM Design

### Validation Byte

A validation byte is stored to determine whether EEPROM contains valid configuration data.

| Address | Value  |
| ------- | ------ |
| `0x00`  | `0xBD` |

On startup:

1. Firmware checks address `0x00`.
2. If value equals `0xBD`, stored configuration is loaded.
3. Otherwise, default configuration values are used and written to EEPROM.

---

### Temperature Limit Storage

| Parameter         | EEPROM Address             |
| ----------------- | -------------------------- |
| Temperature Limit | `0x02` (MSB), `0x01` (LSB) |

The temperature threshold is retained across power cycles.

---

## Logging System

The firmware maintains a circular buffer containing the latest **5 log entries**.

### Logged Events

* `OK → OVERHEAT`
* `OVERHEAT → OK`

Each log entry contains:

* Timestamp (uptime)
* Previous state
* Current state

When the buffer becomes full, the oldest entry is overwritten.

---

## UART Command Protocol

### Supported Commands

| Command       | Description                        |
| ------------- | ---------------------------------- |
| `get_voltage` | Read current voltage               |
| `get_temp`    | Read current temperature           |
| `get_status`  | Read system status                 |
| `get_tlimit`  | Read configured temperature limit  |
| `set_tlimit`  | Update temperature limit           |
| `get_time`    | Read system uptime                 |
| `get_log`     | Retrieve event logs                |
| `clear_log`   | Clear stored logs                  |
| `get_all`     | Retrieve all available information |

---

## State Monitoring

The firmware continuously evaluates the measured temperature against the configured threshold.

```text
Temperature <= Limit
        |
        v
       OK

Temperature > Limit
        |
        v
    OVERHEAT
```

State transitions are automatically logged.

---

## Startup Sequence

```text
Power On
   |
   v
Initialize Drivers
(UART, ADC, EEPROM, Timer)
   |
   v
Validate EEPROM
   |
   +--> Invalid -> Load Defaults
   |
   +--> Valid -> Load Stored Configuration
   |
   v
Start Services
   |
   v
Enter Command Processing Loop
```

---

## Target Hardware

* Microcontroller: PIC16F877A
* Board: Picgenios
* Clock: 20 MHz Crystal
* Communication: UART
* Compiler: XC8
* IDE: MPLAB X

*Note: PicsimLab software used to simulate hardware setup*

## Build Environment

- MPLAB X IDE v5.35+
- XC8 Compiler v3.10+

## Build

1. Open the `firmware` directory in MPLAB X.
2. Ensure XC8 Compiler v3.10+ is installed.
3. Select the default configuration.
4. Build the project using F11.


