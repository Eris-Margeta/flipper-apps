# Reality Dimension Clock for Flipper Zero

A dimensional stability monitoring application that measures electromagnetic ratios across multiple frequency bands to detect anomalies in physical constants.

![Flipper Zero](https://img.shields.io/badge/Flipper%20Zero-FF6600?style=flat&logo=flipper&logoColor=white)
![Version](https://img.shields.io/badge/version-2.1-blue)
![Status](https://img.shields.io/badge/status-experimental-yellow)

## Theoretical Basis

This application is based on the theoretical framework that fundamental physical constants (vacuum permittivity ε₀, vacuum permeability μ₀, and the speed of light c) determine electromagnetic propagation characteristics across frequency bands.

If these constants were to change (as theorized in dimensional shift scenarios), the ratios between signal propagation at different frequencies would deviate from their baseline values.

### The Dimensional Stability Index (Φ)

The app calculates:

```
Φ = (R_LF/HF) / (R_HF/UHF) = (S_LF × S_UHF) / (S_HF²)
```

Where:
- **LF** = 125 kHz band (Low Frequency)
- **HF** = 13.56 MHz band (High Frequency)
- **UHF** = 433 MHz band (Ultra High Frequency)

In a stable dimension, Φ should remain constant.

## Features

- **Multi-Band Analysis**: Measures signal strength across LF, HF, and UHF bands
- **Baseline Calibration**: Establishes your "home dimension" reference
- **Real-Time Monitoring**: Continuous dimensional stability tracking with 1000-sample rolling buffer
- **Drift Detection**: Shows percentage deviation from baseline
- **Brightness Control**: Adjustable screen brightness (0-100%)
- **QR Code Info Screen**: Quick access to source code repository
- **Status Classification**:
  - **HOME**: >90% match - Home dimension confirmed
  - **STABLE**: 75-90% match - Within normal parameters
  - **DRIFT**: 50-75% match - Minor deviations detected
  - **FOREIGN**: <50% match - Significant deviation detected

## Screens

Navigate between screens using LEFT/RIGHT:

1. **Home** - Main display with E-137 dimension code and status
2. **Bands** - Real-time band analysis (LF, HF, UHF) with signal bars
3. **Details** - Scrollable technical details (UP/DOWN to scroll)
4. **Info** - QR code linking to GitHub repository

## Controls

| Button | Action |
|--------|--------|
| OK | Open settings menu |
| LEFT/RIGHT | Navigate screens |
| UP/DOWN | Scroll details / Adjust brightness |
| BACK | Exit application |

### Settings Menu

Press OK to access the settings menu:
- **CALIBRATE** - Recalibrate the dimensional baseline
- **BRIGHTNESS** - Adjust screen brightness (0-100%)

## Known Issues

### Brightness Flicker

When adjusting brightness or pressing buttons, there may be brief flickers to maximum brightness. This is a limitation of the Flipper Zero's notification system, which overrides direct hardware backlight control on input events. The app uses a high-frequency timer (200Hz) to minimize this effect, but some flicker may still be visible.

## Display

```
    REALITY CLOCK

    PHI: 0.8234

      STABLE

    Drift: +0.42%

  LF:42 HF:51 UHF:60
```

## Technical Implementation

### Hardware Used

- **CC1101 Radio** (Sub-GHz): Direct RSSI measurement at 433.92 MHz
- **Simulated Bands**: LF and HF bands are simulated based on theoretical near-field coupling characteristics

### Measurement Process

1. **UHF Band**: Direct RSSI reading from CC1101 radio
2. **HF Band**: Simulated with mid-range coupling factor
3. **LF Band**: Simulated with near-field coupling characteristics
4. **Averaging**: 10 samples per measurement cycle
5. **Calibration**: 50 samples to establish baseline Φ

### Limitations

This is an experimental/theoretical device. True dimensional monitoring would require:
- Dedicated spectrum analyzers for each band
- Atomic clock references for timing stability
- Shielded measurement chambers
- Correlation with fundamental constant measurements

## Building

```bash
cd apps/reality-clock
poetry run ufbt           # Build only
poetry run ufbt launch    # Build + install + run
```

## Technical Details

| Property | Value |
|----------|-------|
| Target | Flipper Zero |
| App Type | External (.fap) |
| Category | Tools |
| Stack Size | 8KB |
| Version | 2.1 |

## Academic Paper

See [paper.html](paper.html) for the full theoretical framework and experimental design.

## Disclaimer

This application is a thought experiment implementation. The "dimensional stability" readings are based on electromagnetic measurements that vary due to normal environmental factors (RF interference, temperature, movement, etc.).

Actual detection of dimensional shifts would require fundamental physics research and is not currently possible with consumer hardware.

## License

MIT License - see [LICENSE](../../LICENSE)

## Author

**Eris Margeta** ([@Eris-Margeta](https://github.com/Eris-Margeta))

## App Catalog Submission

### Missing Items

To submit to the Flipper Application Catalog, the following items are needed:

- [ ] **Update manifest.yml** - Replace `PENDING_COMMIT_HASH` with actual commit SHA after pushing

### Ready Items

- [x] `manifest.yml` - Catalog manifest file (needs commit hash)
- [x] `README.md` - Full documentation
- [x] `changelog.md` - Version history
- [x] `application.fam` - App metadata
- [x] `icon.png` - App icon
- [x] `paper.html` - Academic paper
- [x] `screenshots/` - 7 screenshots (screenshot1-7.png)

---

Part of [flipper-apps](https://github.com/Eris-Margeta/flipper-apps) monorepo.
