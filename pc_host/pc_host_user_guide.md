# PC Host User Guide

The PC Host Application provides a command-line interface (CLI) for communicating with the PIC16F877A firmware over UART.

The application translates user commands into firmware protocol commands, sends them over the serial connection, and displays the received responses.

---

# Command Format

```text
pc_host <command>
```

Examples:

```text
pc_host --voltage

pc_host --temp

pc_host --all
```

Commands are entered from the command line and executed individually.

---

# Configuration File

Before running the application, ensure the configuration file contains valid serial communication settings.

Example:

```text
COM_PORT=COM3
BAUD_RATE=9600
```

The host application reads these values during startup.

---

# General Rules

1. Commands are case-insensitive.
2. Commands must begin with `--`.
3. Command names must match exactly.
4. Only one command may be executed per invocation.
5. Invalid commands are rejected before transmission.
6. The host waits until the firmware sends the response terminator:
    ```text
    ***
    ```
7. Communication requires:

   * Firmware running
   * Correct COM port
   * Matching baud rate

---

# --voltage

## Purpose

Requests the current voltage measurement.

## Command Format

```text
pc_host --voltage
```

## Firmware Request

```text
get_voltage
```

## Example Output

```text
Voltage           : 227.5 V
```


---

# --temp

## Purpose

Requests the current temperature measurement.

## Command Format

```text
pc_host --temp
```

## Firmware Request

```text
get_temp
```

## Example Output

```text
Temperature       : -11 degC
```


---

# --status

## Purpose

Displays the current thermal status.

## Command Format

```text
pc_host --status
```

## Firmware Request

```text
get_status
```

## Example Outputs

```text
Status            : OK
```

or

```text
Status            : OVERHEAT
```


---

# --templimit

## Purpose

Displays the configured temperature threshold.

## Command Format

```text
pc_host --templimit
```

## Firmware Request

```text
get_tlimit
```

## Example Output

```text
Temperature limit : 10.7 degC
```


---

# --set_templimit

## Purpose

Updates the firmware temperature threshold.

## Command Format

```text
pc_host --set_templimit <value>
```

## Example

```text
pc_host --set_templimit 70.5
```

## Firmware Request

```text
set_tlimit 70.5
```

## Successful Response

```text
Temperature limit value updated successfully.
```

## Notes

* Value is stored in EEPROM.
* Setting persists after power cycle.
* Numeric parameter required.
* Within temeperature range(-20 to 100 °C).


---

# --uptime

## Purpose

Displays system uptime.

## Command Format

```text
pc_host --uptime
```

## Firmware Request

```text
get_time
```

## Example Output

```text
Uptime            : 0  hr  14 min  46 secs
```


---

# --log

## Purpose

Displays stored event logs.

## Command Format

```text
pc_host --log
```

## Firmware Request

```text
get_log
```

## Example Output

```text
Log entry   1 |  uptime  0  hr  15 min  9  secs  |  state transition to  : OVERHEAT
Log entry   2 |  uptime  0  hr  15 min  13 secs  |  state transition to  : OK
```

## Notes

* Only state transitions are logged.
* If no logs present then the following response is received:
  ```text
  No log entries found.
  ```


---

# --clear_log

## Purpose

Clears all stored log entries.

## Command Format

```text
pc_host --clear_log
```

## Firmware Request

```text
clear_log
```

## Successful Output

```text
Log entries cleared.
```

## Notes

* If no logs present then the following response is received:
  ```text
  No log entries present to be cleared.
  ```

  
---

# --all

## Purpose

Displays all available monitoring information.

## Command Format

```text
pc_host --all
```

## Firmware Request

```text
get_all
```

## Example Output

```text
Voltage           : 227.5 V
Temperature       : 19.1 degC
Status            : OK
Temperature limit : 75 degC
Uptime            : 0  hr  16 min  1  secs
```


---

# Invalid Commands

### Unknown Command/ Invalid Input

Input:

```text
pc_host --random
```
```text
pc_host asdfdc...
```

Output:

```text
Error: command not found.
```

---

### Invalid input number

Input:

```text
set_tlimit 
```
Input:

```text
set_tlimit 12abc
```

Response:

```text
Error: incorrect command input format.
```

---

### Input number not within range

Input:

```text
set_tlimit 105.6
```

Response:

```text
Error: input number not within range (-20 to 100 degC).
```


---

# Error Response

## For Config file:

### File opening error

Output:

```text
Error: configuration file opening failed.
Command aborted.
```
### File read error

Output:

```text
Error: configuration file read error.
Command aborted.
```

---

## For COM port:

### NO COM Ports detected

Output:

```text
Error: no available COM ports found.
```

### COM Port not available

Output:

```text
Error: configured COM port not available.
```

### COM Port opening fail

Output:

```text
Error: COM port opening failed.
```

---

## For serial communication:

### UART transmission fail 

Output:

```text
Error: serial transmission failed.
```

### UART receive fail 

Output:

```text
Error: serial receive failed.
```

### Firmware response overflow  

Output:

```text
Error: command receive buffer overflow.
```

### Temperature threshold setting fail 

Output:

```text
Error: updation failed; reset node device.
```

---

# Command Summary

| Host Command          | Firmware Command |
| --------------------- | ---------------- |
| --voltage             | get_voltage      |
| --temp                | get_temp         |
| --status              | get_status       |
| --templimit           | get_tlimit       |
| --set_templimit value | set_tlimit value |
| --uptime              | get_time         |
| --log                 | get_log          |
| --clear_log           | clear_log        |
| --all                 | get_all          |

---

# Troubleshooting Checklist

Before reporting a communication issue, verify:

* Firmware is programmed and running.
* UART baud rates match.
* Correct COM port is configured.
* Device is powered.
* Configuration file values are correct.
* Command spelling is correct.

