import re
import time
import serial

PORT = "COM4"
BAUD_RATE = 115200


def read_until_prompt(ser, timeout=3):
    data = bytearray()
    deadline = time.time() + timeout

    while time.time() < deadline:
        waiting = ser.in_waiting
        if waiting:
            data.extend(ser.read(waiting))
            if b"health> " in data:
                break
        else:
            time.sleep(0.05)

    return data.decode("utf-8", errors="replace")


def run_command(ser, command):
    ser.write((command + "\r\n").encode())
    ser.flush()
    return read_until_prompt(ser)


def extract_number(output, label):
    match = re.search(rf"{re.escape(label)}:\s*(\d+)", output)
    assert match, f"Could not find {label} in output:\n{output}"
    return int(match.group(1))


def main():
    with serial.Serial(PORT, BAUD_RATE, timeout=0.2) as ser:
        time.sleep(2)
        ser.reset_input_buffer()

        result = run_command(ser, "fault_clear")
        assert "Fault state cleared" in result, result

        result = run_command(ser, "status")
        assert "Health: OK" in result, result
        uptime_before = extract_number(result, "Uptime")
        print("PASS: Healthy status")

        result = run_command(ser, "fault")
        assert "Simulated fault recorded" in result, result
        print("PASS: Fault injection")

        result = run_command(ser, "status")
        assert "Health: FAULT" in result, result
        print("PASS: Fault status")

        result = run_command(ser, "fault_clear")
        assert "Fault state cleared" in result, result
        print("PASS: Fault clear")

        result = run_command(ser, "stats")
        assert extract_number(result, "Faults") == 0, result
        reports_before = extract_number(result, "Reports")
        print("PASS: Fault counter reset")

        time.sleep(1.5)

        result = run_command(ser, "stats")
        reports_after = extract_number(result, "Reports")
        assert reports_after > reports_before, result
        print("PASS: Monitoring thread increments reports")

        result = run_command(ser, "status")
        uptime_after = extract_number(result, "Uptime")
        assert uptime_after > uptime_before, result
        print("PASS: Uptime increases")

    print("\nAll health-monitor tests passed.")


if __name__ == "__main__":
    main()
