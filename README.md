# Embedded_UART_Monitoring_System

Embedded monitoring system built around the PIC16F877A microcontroller and a desktop host application communicating through a custom UART command protocol.

The firmware acquires voltage and temperature measurements using the ADC subsystem, tracks system uptime, stores configuration data in EEPROM, monitors thermal state transitions, and maintains an event log. A PC-side CLI application provides a user-friendly interface for interacting with the embedded device.

---

## Features

### Firmware
- UART command processing
- ADC-based voltage & voltage monitoring
- EEPROM-backed persistent storage for temperature threshold configuration
- Uptime tracking using Timer1 interrupt
- Event logging with timestamps
- Overheat state monitoring
- Circular buffer log management

### PC Host Application
- Command-line interface
- Serial communication using libserialport
- Command table architecture
- Configuration-file support
- Response parsing and display
- Input validation and error handling

---

## System Architecture
<p align="center">
  <img width="450" height="600" alt="ChatGPT Image Jun 23, 2026, 04_02_04 PM" src="https://github.com/user-attachments/assets/64433c67-abe0-43fd-95cf-0bbc4f584383" />
</p>

---

## Firmware Architecture

### Driver Layer
- UART Driver
- ADC Driver
- EEPROM Driver
- Timer1 Driver

### Service Layer
- Sensor Conversion
- Uptime Tracking
- Logging Service
- State Monitoring

### Application Layer
- Command Parser
- Command Executor
- UART Protocol Handler

---

## UART Protocol: Command Reference

| Command | Description |
|----------|-------------|
| get_voltage | Display voltage reading |
| get_temp | Display temperature reading |
| get_status | Display OK / OVERHEAT status |
| get_tlimit | Display configured temperature limit |
| set_tlimit value | Update temperature limit |
| get_time | Display uptime |
| get_log | Display event log |
| clear_log | Clear event log |
| get_all | Display all system information |

### Response Terminator

All responses end with:

```text
***
```

This allows the host application to determine when a complete response has been received.

---

## EEPROM Design

### Validation Byte

Address:

```text
0x00
```

Value:

```text
0xBD
```

Used to determine whether EEPROM has been initialized.

### Persistent Temperature Limit

Address Pair:

```text
0x0201
```

Stores:

```text
Temperature threshold
```

Default:

```text
70.0 °C
```

---

## Logging System

A circular buffer is used to store system events.

### Log Entry Format

```text
UPTIME: HH:MM:SS   STATE
```

Example:

```text
UPTIME: 0:3:17   OVERHEAT
UPTIME: 0:5:41   OK
```

### Logged Events

- OK → OVERHEAT
- OVERHEAT → OK

Maximum entries:

```text
5
```

Oldest entries are overwritten when the buffer becomes full.

---

## State Monitoring

Temperature readings are continuously compared against the configured threshold.

```text
Temperature Reading
          |
          v
Compare Against Limit
          |
    +-----+-----+
    |           |
    v           v
   OK      OVERHEAT
```

State transitions generate log entries.

---

## Uptime Tracking

Timer1 interrupts are used to accumulate elapsed time.

Output format:

```text
HH:MM:SS
```

Example:

```text
UPTIME: 12:15:42
```

---

## PC Host Commands

| User Command | Firmware Request |
|-------------|------------------|
| --voltage | get_voltage |
| --temp | get_temp |
| --status | get_status |
| --uptime | get_time |
| --templimit | get_tlimit |
| --set_templimit value | set_tlimit value |
| --log | get_log |
| --clear_log | clear_log |
| --all | get_all |

---

## Compilation & Setup 

### Firmware

Tools:
- MPLAB X IDE
- XC8 Compiler

Target:
- Microcontroller: PIC16F877A
- Board : Picgenios

### PC Host

Requirements:
- GCC
- libserialport

### Hardware setup simulation

Tools:
- PicsimLab

---

## Concepts used

- Embedded C
- UART Communication
- ADC Data Acquisition
- EEPROM Persistence
- Interrupt Handling
- Timer Configuration
- Circular Buffers
- Event Logging
- Command Parsing
- Serial Protocol Design
- Host-Firmware Integration
- State Monitoring

---
