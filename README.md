# Embedded Device Health Monitor

A Zephyr RTOS-based embedded firmware project for the Shrike Lite RP2040 + FPGA development board. It provides a serial command-line interface to monitor device health, track uptime, inspect counters, and simulate faults.

## Features

- **Zephyr RTOS firmware** written in C.
- **USB CDC ACM serial console** with a command-line shell.
- Device health and uptime reporting.
- Report and simulated-fault counters.
- Fault injection and fault-state clearing commands.
- Python host-side hardware-in-the-loop tests using PySerial.

## Hardware and Software

- **Board:** Shrike Lite RP2040 + FPGA
- **RTOS:** Zephyr
- **Language:** C
- **Host-side testing:** Python 3 and PySerial
- **Development environment:** Ubuntu on WSL
- **Build system:** CMake and west

## Serial Commands

Connect to the board's serial console at 115200 baud using a terminal such as PuTTY.

| Command | Description |
|---|---|
| `help` | List available commands |
| `status` | Show health state and uptime |
| `stats` | Show report and fault counters |
| `fault` | Simulate a fault |
| `fault_clear` | Clear the simulated fault state |

## Build Firmware

Activate the Python environment and build from the Zephyr workspace:

```bash
source ~/zephyr-venv/bin/activate
cd ~/zephyrproject
west build -p always -b shrike_lite ~/embedded-health-monitor -d ~/embedded-health-monitor/build
```

The firmware UF2 is generated at:

`~/embedded-health-monitor/build/zephyr/zephyr.uf2`

Flash it using the board's supported USB bootloader procedure.

## Run Host-Side Tests

The host-side tests communicate with the board over its serial console. Update the `PORT` setting in `tests/test_health_monitor.py` if your COM port differs.

Install PySerial on the Windows host if needed:

```powershell
python -m pip install pyserial
```

Close PuTTY or any other serial terminal before running the test script, since only one application can normally own the COM port at a time.

Run the tests from PowerShell:

```powershell
python C:\Users\Public\test_health_monitor.py
```

The test script checks healthy status, fault injection, fault status, fault clearing, and counter reset.

**Verified milestone:** all five host-side automated tests passed on the development board.

## Unit Tests

The health-monitor logic is tested using Zephyr ztest on the `native_sim` target.

Run the tests:

```bash
source ~/zephyr-venv/bin/activate
cd ~/zephyrproject
west build -p always -b native_sim ~/embedded-health-monitor/tests/unit -d ~/embedded-health-monitor/build-unit
west build -t run -d ~/embedded-health-monitor/build-unit
```

Unit-test coverage:
- Initial state is healthy
- Recording a fault changes the health state
- Multiple faults are counted
- Clearing faults restores the healthy state

**Verified:** 4/4 ztest cases passed.

## Testing

### Zephyr Unit Tests

The health-monitor logic is tested using Zephyr ztest on `native_sim`.

Run the tests:

```bash
source ~/zephyr-venv/bin/activate
cd ~/zephyrproject
west build -p always -b native_sim ~/embedded-health-monitor/tests/unit -d ~/embedded-health-monitor/build-unit
west build -t run -d ~/embedded-health-monitor/build-unit
```

The four unit tests verify:
- Initial healthy state
- Fault recording
- Multiple fault counting
- Fault clearing

### Python Hardware-in-the-Loop Tests

The Python test suite communicates with the physical board through USB serial using PySerial.

It verifies:
- Healthy status
- Fault injection and fault status
- Fault clearing and counter reset
- Report counter increases over time
- Uptime increases between status requests

Run on Windows with the board's serial port available and PuTTY closed:

```powershell
python C:\Users\Public\test_health_monitor.py
```

The test script currently expects `COM4` at 115200 baud. Update its `PORT` setting if Windows assigns another port.

### Verification Notes

Record the actual results from each test run. Do not describe tests as passing unless they have been run successfully against the current firmware.



## Project Status

This is an incremental embedded systems learning project. Planned improvements include RTOS task separation, C unit tests with Zephyr ztest, improved counter synchronization, and expanded fault scenarios.

## Latest Verification Results

**Verified on the current development version:**

- Zephyr `native_sim` unit tests: **5 passed, 0 failed**
- Python serial hardware-in-the-loop checks: **7 passed**
- Board serial connection: tested at 115200 baud
- Monitoring thread: report counter increases over time
- Health state: fault injection and clearing verified
- Fault counter: overflow protection covered by a unit test

The unit tests exercise the health-monitor C module in isolation. The Python tests communicate with the physical development board through its serial shell.

These results apply to the version tested; rerun both test suites after subsequent code changes.

