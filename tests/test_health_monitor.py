
import time
import serial


PORT = "COM4"
BAUD_RATE = 115200


def read_until_prompt(ser, timeout=3):
    """Read serial output until the health-monitor prompt appears."""
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


def main():
    with serial.Serial(PORT, BAUD_RATE, timeout=0.2) as ser:
        time.sleep(2)
        ser.reset_input_buffer()

        # Clear any simulated fault before testing.
        run_command(ser, "fault_clear")

        result = run_command(ser, "status")
        assert "Health: OK" in result, result
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
        assert "Faults: 0" in result, result
        print("PASS: Fault counter reset")

    print("\nAll health-monitor tests passed.")


if __name__ == "__main__":
    main()

