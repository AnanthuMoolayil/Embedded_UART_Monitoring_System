# Firmware Command User Guide

### General Rules

Before using any command, note the following:

1. Commands are case-insensitive.
3. Commands use underscore separators (`_`).
4. Commands are transmitted as ASCII strings.
5. Responses are terminated by:

```text
***
```

6. Commands must match the supported syntax exactly.
7. Leading or trailing spaces are not recommended.
8. Voltage & Temperature values are represented with one decimal place.
10. Temperature limit values entered must be:<br>
    a. Whole numbers or  numbers with one decimal place.<br>
    b. Within temeperature range(-20 to 100 °C)



---

## get_voltage

### Purpose

Retrieves the latest voltage measurement.

### Command Format

```text
get_voltage
```

### Example Response

```text
VOLTAGE: 12.4
***
```


---

## get_temp

### Purpose

Retrieves the latest temperature measurement.

### Command Format

```text
get_temp
```

### Example Response

```text
TEMP: 36.7
***
```

---

## get_status

### Purpose

Displays current thermal state.

### Command Format

```text
get_status
```

### Example Response

```text
STATUS: OK
***
```

or

```text
STATUS: OVERHEAT
***
```

### Notes

The state depends on the configured temperature limit.

---

## get_tlimit

### Purpose

Displays the currently configured temperature threshold.

### Command Format

```text
get_tlimit
```

### Example Response

```text
TEMP LIMIT: 70.0
***
```

---

## set_tlimit

### Purpose

Updates the temperature threshold and stores it in EEPROM.

### Command Format

```text
set_tlimit <value>
```

### Example

```text
set_tlimit 70.0
```

### Successful Response

```text
DONE
***
```

### Notes

* Value is stored in EEPROM.
* Persists after power cycle.
* Must be a whole number or  number with one decimal place.
* Within temeperature range(-20 to 100 °C).


---

## get_time

### Purpose

Displays system uptime.

### Command Format

```text
get_time
```

### Example Response

```text
UPTIME: 12:34:56
***
```

---

## get_log

### Purpose

Displays stored event log entries.

### Command Format

```text
get_log
```

### Example Response

```text
UPTIME: 0:03:17 OVERHEAT
UPTIME: 0:05:41 OK
***
```

### Notes

* Only state transitions are logged.
* If no logs present then the following response is received:
  ```text
  NO LOGS PRESENT
  ```

---

## clear_log

### Purpose

Clears all stored log entries.

### Command Format

```text
clear_log
```

### Successful Response

```text
DONE
***
```
### Notes

* If no logs present then the following response is received:
  ```text
  NO LOGS PRESENT
  ```
---

## get_all

### Purpose

Displays all monitored parameters.

### Command Format

```text
get_all
```

### Example Response

```text
VOLTAGE: 12.4
TEMP: 35.8
STATUS: OK
TEMP LIMIT: 70.0
UPTIME: 01:15:42
***
```

---

# Invalid Commands

### Unsupported Command

Input:

```text
get_random
```

Response:

```text
Command not found!
***
```

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
Invalid input number!
***
```

### Incorrect decimal place

Input:

```text
set_tlimit 80.8956
```

Response:

```text
Input number cannot have decimal place exceeding 1!
***
```

### Input number not within range

Input:

```text
set_tlimit 105.6
```

Response:

```text
Input number not within set range!
***
```

---

# Error Response

### EEPROM write error

Response:

```text
eeprom load failed; restart!
***
```

---

# Protocol Summary

| Category            | Requirement       |
| ------------------- | ----------------- |
| Command Case        | case insensitive    |
| Response Terminator | `***`             |
| Parameter Separator | Space             |
| Decimal Display     | One decimal place |
| Persistent Settings | Temperature Limit |
| Log Capacity        | 5 entries         |
| State Values        | OK / OVERHEAT     |
