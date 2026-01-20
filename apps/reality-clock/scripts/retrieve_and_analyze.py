#!/usr/bin/env python3
"""
Retrieve sensor log from Flipper Zero and analyze RSSI data.
This script connects to the Flipper via serial, downloads the log file,
and performs statistical analysis to determine optimal constants.
"""

import os
import sys
import time
import glob
import csv
import statistics
from pathlib import Path

# Find Flipper serial port
def find_flipper_port():
    """Find the Flipper Zero's serial port."""
    patterns = [
        '/dev/cu.usbmodemflip*',  # macOS
        '/dev/ttyACM*',  # Linux
        'COM*',  # Windows
    ]
    for pattern in patterns:
        ports = glob.glob(pattern)
        for port in ports:
            if 'flip' in port.lower():
                return port
        if ports:
            return ports[0]
    return None

def download_log_via_cli(output_path):
    """
    Download the sensor log using Flipper CLI.
    Note: This requires the app to be closed so the file is synced.
    """
    import serial

    port = find_flipper_port()
    if not port:
        print("ERROR: Flipper Zero not found. Please connect it via USB.")
        return False

    print(f"Found Flipper at: {port}")

    try:
        ser = serial.Serial(port, 230400, timeout=5)
        time.sleep(0.5)

        # Send storage read command
        cmd = b"storage read /ext/apps_data/reality_clock/sensor_log.csv\r\n"
        ser.write(cmd)
        ser.flush()

        # Read response
        time.sleep(1)
        data = ser.read(ser.in_waiting or 1)

        # Keep reading until we get all data
        while True:
            time.sleep(0.5)
            more = ser.read(ser.in_waiting or 1)
            if not more:
                break
            data += more

        ser.close()

        # Parse the response
        text = data.decode('utf-8', errors='ignore')

        # Find the CSV content (between "Size:" line and next prompt)
        lines = text.split('\n')
        csv_lines = []
        in_csv = False

        for line in lines:
            if line.startswith('timestamp_ms'):
                in_csv = True
            if in_csv:
                if line.strip().startswith('>') or 'storage' in line.lower():
                    break
                if line.strip():
                    csv_lines.append(line.strip())

        if csv_lines:
            with open(output_path, 'w') as f:
                f.write('\n'.join(csv_lines))
            print(f"Downloaded {len(csv_lines)} lines to {output_path}")
            return True
        else:
            print("No CSV data found in response")
            return False

    except Exception as e:
        print(f"Error: {e}")
        return False

def analyze_log(csv_path):
    """Analyze the sensor log and determine optimal constants."""

    if not os.path.exists(csv_path):
        print(f"Log file not found: {csv_path}")
        return None

    print(f"\n{'='*60}")
    print("SENSOR DATA ANALYSIS")
    print(f"{'='*60}\n")

    # Read CSV
    data = {
        'rssi_315': [],
        'rssi_433': [],
        'rssi_868': [],
        'temperature': [],
        'voltage': [],
        'phi_current': [],
        'match_pct': []
    }

    with open(csv_path, 'r') as f:
        reader = csv.DictReader(f)
        for row in reader:
            try:
                for key in data.keys():
                    if key in row:
                        data[key].append(float(row[key]))
            except (ValueError, KeyError):
                continue

    total_samples = len(data['rssi_315'])
    duration_sec = total_samples  # 1 sample/sec

    print(f"Total Samples: {total_samples}")
    print(f"Duration: {duration_sec} seconds ({duration_sec/60:.1f} minutes)")
    print()

    # Analyze each band
    print("RSSI STATISTICS (dBm):")
    print("-" * 50)

    bands = [
        ('315 MHz', 'rssi_315'),
        ('433 MHz', 'rssi_433'),
        ('868 MHz', 'rssi_868'),
    ]

    band_stats = {}
    for name, key in bands:
        if data[key]:
            avg = statistics.mean(data[key])
            std = statistics.stdev(data[key]) if len(data[key]) > 1 else 0
            min_val = min(data[key])
            max_val = max(data[key])
            band_stats[key] = {'avg': avg, 'std': std, 'min': min_val, 'max': max_val}
            print(f"{name:12} Avg: {avg:7.2f}  Std: {std:5.2f}  Range: [{min_val:.1f}, {max_val:.1f}]")

    print()

    # Temperature
    if data['temperature']:
        temp_avg = statistics.mean(data['temperature'])
        temp_std = statistics.stdev(data['temperature']) if len(data['temperature']) > 1 else 0
        print(f"Temperature: Avg: {temp_avg:.1f}°C  Std: {temp_std:.2f}°C")

    # Voltage
    if data['voltage']:
        volt_avg = statistics.mean(data['voltage'])
        print(f"Battery:     Avg: {volt_avg:.3f}V")

    print()

    # PHI Analysis
    if data['phi_current']:
        phi_avg = statistics.mean(data['phi_current'])
        phi_std = statistics.stdev(data['phi_current']) if len(data['phi_current']) > 1 else 0
        phi_min = min(data['phi_current'])
        phi_max = max(data['phi_current'])

        print("PHI ANALYSIS:")
        print("-" * 50)
        print(f"PHI Average:  {phi_avg:.6f}")
        print(f"PHI Std Dev:  {phi_std:.6f}")
        print(f"PHI Range:    [{phi_min:.6f}, {phi_max:.6f}]")
        print(f"Variation:    {(phi_std/phi_avg*100):.2f}%")

    print()

    # RECOMMENDATIONS
    print("RECOMMENDED CONSTANTS:")
    print("=" * 60)

    if band_stats:
        print("\n/* Real sensor base values (from collected data) */")
        for name, key in bands:
            if key in band_stats:
                print(f"#define BASE_{key.upper().replace('RSSI_', '')}  {band_stats[key]['avg']:.1f}f  /* Avg RSSI at {name} */")

        print("\n/* Variance for each band */")
        for name, key in bands:
            if key in band_stats:
                var = band_stats[key]['std'] * 2  # 2-sigma range
                print(f"#define VAR_{key.upper().replace('RSSI_', '')}   {var:.1f}f  /* 2-sigma variation */")

    if data['phi_current']:
        print(f"\n/* PHI baseline (use this as the 'home' dimension baseline) */")
        print(f"#define PHI_BASELINE     {phi_avg:.6f}f")
        print(f"#define PHI_TOLERANCE    {phi_std * 2:.6f}f  /* 2-sigma for HOME threshold */")

        # Calculate appropriate thresholds
        # HOME: within 1-sigma, STABLE: within 2-sigma, DRIFT: within 3-sigma
        home_thresh = 100.0 - (phi_std / phi_avg * 100)
        stable_thresh = 100.0 - (phi_std * 2 / phi_avg * 100)
        drift_thresh = 100.0 - (phi_std * 3 / phi_avg * 100)

        print(f"\n/* Recommended thresholds based on variance */")
        print(f"#define HOME_THRESHOLD     {max(home_thresh, 95.0):.1f}f")
        print(f"#define STABLE_THRESHOLD   {max(stable_thresh, 85.0):.1f}f")
        print(f"#define UNSTABLE_THRESHOLD {max(drift_thresh, 70.0):.1f}f")

    print("\n" + "=" * 60)

    return {
        'samples': total_samples,
        'duration_sec': duration_sec,
        'bands': band_stats,
        'phi_avg': phi_avg if data['phi_current'] else None,
        'phi_std': phi_std if data['phi_current'] else None
    }

def main():
    script_dir = Path(__file__).parent
    app_dir = script_dir.parent
    log_path = app_dir / "sensor_log.csv"

    print("Reality Clock Sensor Data Analyzer")
    print("=" * 60)

    # Check if we have a local copy already
    if log_path.exists():
        print(f"Found existing log at: {log_path}")
        choice = input("Use existing file? (y/n): ").strip().lower()
        if choice != 'y':
            print("\nPlease copy sensor_log.csv from your Flipper's SD card:")
            print("  /ext/apps_data/reality_clock/sensor_log.csv")
            print(f"  to: {log_path}")
            return
    else:
        print("No local log file found.")
        print("\nTo collect data:")
        print("1. Run the Reality Clock app on your Flipper")
        print("2. Let it collect data for at least 5-10 minutes")
        print("3. Exit the app (press BACK)")
        print("4. Copy the log file from Flipper SD card:")
        print("   /ext/apps_data/reality_clock/sensor_log.csv")
        print(f"   to: {log_path}")
        print("\nTrying to download via CLI...")

        if download_log_via_cli(str(log_path)):
            print("Download successful!")
        else:
            print("\nCould not download automatically.")
            print("Please manually copy the file.")
            return

    # Analyze the data
    results = analyze_log(str(log_path))

    if results and results['samples'] < 300:
        print(f"\nWARNING: Only {results['samples']} samples collected.")
        print("For accurate analysis, collect at least 300-600 samples (5-10 minutes).")

if __name__ == "__main__":
    main()
