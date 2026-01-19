# Reality Dimension Clock for Flipper Zero

A dimensional stability monitoring application that measures electromagnetic ratios across multiple frequency bands to detect anomalies in physical constants.

![Flipper Zero](https://img.shields.io/badge/Flipper%20Zero-FF6600?style=flat&logo=flipper&logoColor=white)
![Version](https://img.shields.io/badge/version-1.0-blue)
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
- **Real-Time Monitoring**: Continuous dimensional stability tracking
- **Drift Detection**: Shows percentage deviation from baseline
- **Status Classification**:
  - **STABLE**: <5% drift - Normal operation
  - **FLUCTUATING**: 5-15% drift - Minor deviations detected
  - **ANOMALY**: >15% drift - Significant deviation detected

## Controls

| Button | Action |
|--------|--------|
| OK | Recalibrate baseline |
| BACK | Exit application |

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
| Stack Size | 4KB |
| Version | 1.0 |

## Academic Paper

See [paper.html](paper.html) for the full theoretical framework and experimental design.

## Disclaimer

This application is a thought experiment implementation. The "dimensional stability" readings are based on electromagnetic measurements that vary due to normal environmental factors (RF interference, temperature, movement, etc.).

Actual detection of dimensional shifts would require fundamental physics research and is not currently possible with consumer hardware.

## License

MIT License - see [LICENSE](../../LICENSE)

## Author

**Eris Margeta** ([@Eris-Margeta](https://github.com/Eris-Margeta))

---

Part of [flipper-apps](https://github.com/Eris-Margeta/flipper-apps) monorepo.
