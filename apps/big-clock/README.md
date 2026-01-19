# Big Clock for Flipper Zero

A full-screen digital bedside/tableside clock for Flipper Zero with adjustable screen brightness.

![Flipper Zero](https://img.shields.io/badge/Flipper%20Zero-FF6600?style=flat&logo=flipper&logoColor=white)
![Version](https://img.shields.io/badge/version-1.1-blue)
![CI](https://github.com/Eris-Margeta/flipper-apps/actions/workflows/ci.yml/badge.svg)

## Use Case

Turn your Flipper Zero into a **bedside or desk clock** with:
- Large, easy-to-read digits visible from across the room
- Adjustable brightness from completely off (0%) to your system maximum
- Perfect for nightstands, desks, or anywhere you need a simple clock

## Features

- **Large Display**: Full-screen time display with custom 24x48 pixel digits
- **Adjustable Brightness**: 11 levels from 0% (off) to 100% in 10% increments
- **Visual Feedback**: Shows brightness bar and percentage when adjusting
- **Always On**: Backlight stays on while the app is running (no timeout)
- **Flicker-Free**: Smooth brightness transitions without screen flashing

## Controls

| Button | Action |
|--------|--------|
| UP | Increase brightness (+10%) |
| DOWN | Decrease brightness (-10%, down to 0% = off) |
| BACK | Exit application |

## Brightness Control

> **Important Note**: This app controls brightness as a **percentage of your system's maximum backlight setting**.
>
> The maximum brightness is determined by:
> **Settings > LCD and Notifications > LCD Backlight**
>
> For example:
> - If your system backlight is set to 50%, then 100% in this app = 50% actual brightness
> - If your system backlight is set to 100%, then 100% in this app = full brightness
>
> **0% brightness** turns the backlight completely off (screen blank but clock still running).

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
# Using Poetry (recommended)
poetry run ufbt           # Build only
poetry run ufbt launch    # Build + install + run

# Using pip
pip install ufbt
ufbt launch
```

### Output

The compiled `.fap` file will be in the `dist/` directory.

## Installation

### Via ufbt (recommended)
```bash
poetry run ufbt launch
```

### Manual Installation
1. Build the app with `ufbt`
2. Copy `dist/big_clock.fap` to your Flipper Zero's SD card at `/ext/apps/Tools/`

## Technical Details

| Property | Value |
|----------|-------|
| Target | Flipper Zero |
| App Type | External (.fap) |
| Category | Tools |
| Stack Size | 2KB |
| Version | 1.1 |

### Implementation Notes

- Uses direct hardware control via `furi_hal_light_set()` for brightness
- Brightness levels mapped from 0-100% to hardware values (0-255)
- Continuously maintains brightness to prevent system timeout

## Version History

See [changelog.md](changelog.md) for full version history.

Current version: see [VERSION](VERSION)

## Known Issues

- **Brief flicker on button press/release**: When adjusting brightness, a brief flicker may occur at the moment of button press and release. This appears to be a hardware/firmware limitation related to how the Flipper Zero's backlight system handles brightness changes. The brightness change itself works correctly.

## License

MIT License - see [LICENSE](../../LICENSE)

## Author

**Eris Margeta** ([@Eris-Margeta](https://github.com/Eris-Margeta))

---

Part of [flipper-apps](https://github.com/Eris-Margeta/flipper-apps) monorepo.
