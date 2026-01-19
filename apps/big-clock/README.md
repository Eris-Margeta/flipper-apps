# Big Clock for Flipper Zero

A full-screen digital clock application for Flipper Zero with adjustable backlight brightness.

![Flipper Zero](https://img.shields.io/badge/Flipper%20Zero-FF6600?style=flat&logo=flipper&logoColor=white)

## Features

- **Large Display**: Full-screen time display with custom 24x48 pixel digits
- **Adjustable Brightness**: 10 brightness levels (10% to 100%)
- **Visual Feedback**: Shows brightness bar and percentage when adjusting
- **Always On**: Backlight stays on while the app is running

## Controls

| Button | Action |
|--------|--------|
| UP | Increase brightness (+10%) |
| DOWN | Decrease brightness (-10%) |
| BACK | Exit application |

## Screenshots

```
    ██  ██████    ██████  ██████
    ██      ██  ●     ██  ██
    ██  ██████  ●  █████  ██████
    ██  ██          ██        ██
    ██  ██████    █████  ██████
```

## Building

### Prerequisites

- [ufbt](https://github.com/flipperdevices/flipperzero-ufbt) (micro Flipper Build Tool)

### Build Commands

```bash
# Install ufbt if not already installed
pip install ufbt

# Build the application
ufbt

# Build and install to connected Flipper Zero
ufbt launch
```

### Output

The compiled `.fap` file will be in the `dist/` directory.

## Installation

### Via ufbt (recommended)
```bash
ufbt launch
```

### Manual Installation
1. Build the app with `ufbt`
2. Copy `dist/big_clock.fap` to your Flipper Zero's SD card at `/ext/apps/Tools/`

## Technical Details

- **Target**: Flipper Zero
- **App Type**: External (.fap)
- **Category**: Tools
- **Stack Size**: 2KB
- **API Version**: 87.1

### Brightness Control

The app uses pre-defined static notification sequences for brightness control to ensure memory safety. Brightness levels are mapped from 10-100% to hardware values (25-255).

## License

MIT License - see [LICENSE](../../LICENSE)

## Author

**Eris Margeta** ([@Eris-Margeta](https://github.com/Eris-Margeta))

---

Part of [flipper-apps](https://github.com/Eris-Margeta/flipper-apps) monorepo.
