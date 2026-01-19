#!/usr/bin/env python3
"""
Retrieve log file from Flipper Zero via serial CLI.
"""

import serial
import time
import sys

SERIAL_PORT = "/dev/cu.usbmodemflip_Untal1ng1"
BAUD_RATE = 230400
LOG_FILE = "/ext/apps_data/reality_clock/debug_log.csv"
OUTPUT_FILE = "/Users/kovachevich/DEV-25/big-clock/apps/reality-clock/debug_log.csv"

def send_command(ser, cmd, timeout=5.0):
    """Send command and wait for response."""
    # Clear any pending data
    ser.reset_input_buffer()

    # Send command
    ser.write(f"{cmd}\r\n".encode())
    ser.flush()

    # Wait for response
    response = b""
    start_time = time.time()

    while time.time() - start_time < timeout:
        if ser.in_waiting:
            chunk = ser.read(ser.in_waiting)
            response += chunk
            # Check for prompt (>:)
            if b">:" in response:
                break
        time.sleep(0.01)

    return response.decode("utf-8", errors="replace")

def main():
    print(f"Connecting to Flipper at {SERIAL_PORT}...")

    try:
        ser = serial.Serial(
            port=SERIAL_PORT,
            baudrate=BAUD_RATE,
            timeout=1.0,
            write_timeout=1.0
        )
    except serial.SerialException as e:
        print(f"Error opening serial port: {e}")
        sys.exit(1)

    time.sleep(0.5)

    # Get initial prompt
    ser.write(b"\r\n")
    time.sleep(0.2)
    ser.read(ser.in_waiting)

    # Check if file exists
    print(f"\nChecking for log file: {LOG_FILE}")
    response = send_command(ser, f"storage stat {LOG_FILE}")
    print(response)

    if "Error" in response or "not found" in response.lower():
        print("Log file not found. The app may not have written data yet.")
        ser.close()
        sys.exit(1)

    # Read the file
    print(f"\nReading log file...")
    response = send_command(ser, f"storage read {LOG_FILE}", timeout=30.0)

    # Parse the file content (skip command echo and prompt)
    lines = response.split("\n")

    # Find CSV data (starts with header or number)
    csv_lines = []
    in_data = False
    for line in lines:
        line = line.strip()
        if line.startswith("sample,") or (in_data and (line and line[0].isdigit())):
            in_data = True
            csv_lines.append(line)
        elif in_data and (">:" in line or line.startswith("storage")):
            break

    if csv_lines:
        print(f"\nRetrieved {len(csv_lines)} lines of data")

        # Save to local file
        with open(OUTPUT_FILE, "w") as f:
            f.write("\n".join(csv_lines) + "\n")

        print(f"Saved to: {OUTPUT_FILE}")

        # Show first few lines
        print("\nFirst 10 lines:")
        for line in csv_lines[:10]:
            print(f"  {line}")

        print(f"\n... ({len(csv_lines)} total lines)")
    else:
        print("No CSV data found in response")
        print("Raw response:")
        print(response[:2000])

    ser.close()

if __name__ == "__main__":
    main()
