# PC Host Application

Desktop CLI application built around **libserialport** for interacting with the embedded target over UART.

---

## Features

* Command parsing and validation
* UART communication using libserialport
* Configuration file support
* Response handling and formatting
* Command table architecture
* Firmware interaction through a simple CLI

---

## User Commands

| Command                   | Description                       |
| ------------------------- | --------------------------------- |
| `--voltage`               | Read measured voltage             |
| `--temp`                  | Read temperature                  |
| `--status`                | Display system status             |
| `--uptime`                | Display device uptime             |
| `--templimit`             | Read configured temperature limit |
| `--set_templimit <value>` | Update temperature limit          |
| `--log`                   | Display stored log entries        |
| `--clear_log`             | Clear log entries                 |
| `--all`                   | Display all available information |

---

## Communication Flow

```text
User
 │
 ▼
PC Host CLI
 │
 ▼
UART Request
 │
 ▼
Embedded Firmware
 │
 ▼
Response Processing
 │
 ▼
Formatted Output
```

---

## Build

```bash
gcc -std=c17 \
    -Wall -Wextra \
    -Wshadow \
    -Wconversion \
    -Wsign-conversion \
    -g \
    -fsanitize=address,undefined \
    1_source_files/*.c -o pc_host -lserailport
```

---

## Run (Example Query)

```bash
./pc_host --temp

Temperature       : -11 degC
```

---

## Requirements

* GCC (C17 compatible)
* libserialport



