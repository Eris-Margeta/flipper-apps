# Changelog

All notable changes to Reality Clock will be documented in this file.

## [2.1] - 2026-01-19

### Added
- **Settings Menu** - Press OK to open settings instead of direct calibration
  - CALIBRATE option to reset dimensional baseline
  - BRIGHTNESS slider (0-100%) with persistence during session
- **QR Code Info Screen** - Navigate right from Details to access
  - Scannable QR code linking to GitHub source repository
  - 2x scaled display for reliable scanning
- **Brightness Control** - Hardware backlight control via furi_hal_light_set
  - High-frequency (200Hz) timer to maintain brightness
  - Input callback reapplication to minimize flicker

### Known Issues
- Brief brightness flicker may occur on button press/release due to Flipper Zero's notification system overriding direct hardware backlight control

## [2.0] - 2026-01-19

### Added
- **Rolling 1000-sample buffer** for ultra-stable readings
  - Each band (LF, HF, UHF) maintains a circular buffer of last 1000 readings
  - Φ calculated from averaged buffer values, not raw readings
  - Guarantees consistent HOME DIMENSION status
- **Sci-Fi UI redesign** for homescreen
  - Decorative corner brackets
  - Large blocky "E-137" dimension number
  - Clean "[HOME DIMENSION]" status display
  - Horizontal accent lines
- **Dynamic sample rates**
  - 5Hz (200ms) during calibration for fast startup
  - 1Hz (1000ms) during normal operation

### Changed
- Homescreen now shows only: title, E-137, and status (no clutter)
- Screens 1 (Home) and 2 (Bands) are NOT scrollable
- Only screen 3 (Details) supports scrolling
- Calibration requires 100 samples (20 seconds at 5Hz)
- Match thresholds: HOME >90%, STABLE >75%, UNSTABLE >50%

### Technical
- State includes raw readings AND averaged readings
- Buffer uses running sum for O(1) average calculation
- Press OK to recalibrate (clears all buffers)

## [1.3] - 2026-01-19

### Added
- New default DIMENSION screen showing large E-137 coordinate
- Three-screen navigation: DIMENSION (default) -> BANDS -> DETAILS

### Changed
- Drastically reduced sensor fluctuation for stable HOME readings
  - LF band: ±4dB -> ±0.4dB
  - HF band: ±3dB -> ±0.3dB
  - UHF band: ±5dB -> ±0.5dB
  - IR band: ±2.5dB -> ±0.3dB
  - Temperature: ±2°C -> ±0.2°C
- Improved dimension status display with clear E-137 home indicator

## [1.2] - 2026-01-19

### Added
- Rolling baseline system: 100-sample batches
- After each batch, baseline updates automatically
- Real battery voltage/current readings via FuelGauge IC
- Batch counter and total sample tracking

### Changed
- Sample rate reduced to 1 per second for stability
- Thresholds: HOME >95%, STABLE >85%, DRIFT >70%, FOREIGN <70%

## [1.1] - 2026-01-19

### Added
- Multi-screen interface (LEFT/RIGHT navigation)
- Scrollable details screen
- Status classification with generous thresholds

## [1.0] - 2025-01-19

### Added
- Initial release
- Multi-band electromagnetic ratio analysis (LF/HF/UHF)
- Dimensional stability index (Φ) calculation
- Baseline calibration with 50-sample averaging
- Real-time drift percentage display
- Status classification (STABLE/FLUCTUATING/ANOMALY)
- Sub-GHz RSSI measurement via CC1101 radio
- Simulated LF and HF band readings
- Recalibration via OK button
- Academic paper documenting theoretical framework
